/* src/engine/camera.c */
/*
 * cpcraft-port — engine/camera.c
 *
 * 3D-to-2D projection implementation (16.16 fixed-point, no floats).
 *
 * Steps:
 *   1. Translate world point by -eye.
 *   2. Rotate by -yaw around Y, then -pitch around X.
 *   3. The resulting (cx, cy, cz) is in camera space.
 *   4. If cz <= near, the point is behind the camera -> not visible.
 *   5. Project: sx = FB_W/2 + (cx / cz) * focal; sy = FB_H/2 - (cy / cz) * focal.
 *
 * All math is fix16_t. Sin/cos come from the BRAD lookup table in
 * math_lut.h — a single table lookup per axis, no branches.
 *
 * The focal length is precomputed at init time from the FOV:
 *   focal = (FB_W / 2) / tan(hfov / 2)
 * For 70° hfov: focal ≈ 114 (in fix16: 114 * 65536 ≈ 7471104).
 */
#include "camera.h"
#include "framebuffer.h"
#include "math_lut.h"
#include "fix16.h"

#define NEAR_PLANE  6554    /* 0.1 in fix16 */

/* Focal length in fix16. For 70° FOV:
 * focal = 80 / tan(35°) = 80 / 0.7002 ≈ 114.2
 * In fix16: 114 * 65536 + (0.2 * 65536) ≈ 7475200 */
static fix16_t focal_length;

void camera_init(void)
{
    /* tan(35°) in fix16. 35° in BRAD = 35 * 65536 / 360 = 6370.
     * sin(35°) ≈ 0.5736, cos(35°) ≈ 0.8192
     * tan(35°) = sin/cos ≈ 0.7002
     * In fix16: 0.7002 * 65536 ≈ 45879 */
    fix16_t s = fix16_sin_brads(deg_to_brads(35));
    fix16_t c = fix16_cos_brads(deg_to_brads(35));
    fix16_t tan35 = fix16_div(s, c);

    /* focal = (FB_W / 2) / tan35 = 80 / 0.7002 ≈ 114.2 */
    focal_length = fix16_div(fix16_from_int(FB_W / 2), tan35);
}

ScreenPoint camera_project(fix16_t wx, fix16_t wy, fix16_t wz,
                           fix16_t ex, fix16_t ey, fix16_t ez,
                           uint16_t yaw, int16_t pitch)
{
    ScreenPoint out = {0, 0, 0, false};

    /* Step 1: translate. */
    fix16_t dx = wx - ex;
    fix16_t dy = wy - ey;
    fix16_t dz = wz - ez;

    /* Step 2: rotate. We rotate the WORLD by -yaw and -pitch, which is
     * equivalent to rotating the camera by +yaw and +pitch.
     *
     * For the yaw rotation (around Y), we need cos(-yaw) and sin(-yaw).
     * Since cos(-x) = cos(x) and sin(-x) = -sin(x), we use:
     *   cy = cos(yaw), sy = sin(yaw)
     * and the rotation matrix becomes:
     *   cx1 =  dx * cy + dz * sy
     *   cz1 = -dx * sy + dz * cy
     *   cy1 =  dy
     */
    fix16_t cy = fix16_cos_brads(yaw);
    fix16_t sy = fix16_sin_brads(yaw);

    fix16_t cx1 = fix16_mul(dx, cy) + fix16_mul(dz, sy);
    fix16_t cy1 = dy;
    fix16_t cz1 = fix16_mul(-dx, sy) + fix16_mul(dz, cy);

    /* Pitch rotation (around X). Same trick: use cos(pitch), -sin(pitch).
     *   cy2 = cy1 * cp - cz1 * sp
     *   cz2 = cy1 * sp + cz1 * cp
     *   cx2 = cx1
     */
    /* Pitch is int16_t; convert to uint16_t for the LUT (it handles the
     * sign via wraparound). */
    fix16_t cp = fix16_cos_brads((uint16_t)pitch);
    fix16_t sp = fix16_sin_brads((uint16_t)pitch);

    fix16_t cx2 = cx1;
    fix16_t cy2 = fix16_mul(cy1, cp) - fix16_mul(cz1, sp);
    fix16_t cz2 = fix16_mul(cy1, sp) + fix16_mul(cz1, cp);

    /* Step 3: camera space is (cx2, cy2, cz2). +Z is forward. */
    if (cz2 <= NEAR_PLANE) {
        out.visible = false;
        return out;
    }

    /* Step 4: perspective project.
     *   sx = FB_W/2 + (cx2 / cz2) * focal
     *   sy = FB_H/2 - (cy2 / cz2) * focal
     *
     * We compute cx2/cz2 as a fix16 division, then multiply by focal. */
    fix16_t scale = fix16_div(FIX16_ONE, cz2);  /* 1/cz2 */
    fix16_t sx_f = fix16_from_int(FB_W / 2) + fix16_mul(fix16_mul(cx2, scale), focal_length);
    fix16_t sy_f = fix16_from_int(FB_H / 2) - fix16_mul(fix16_mul(cy2, scale), focal_length);

    out.sx = fix16_to_int_round(sx_f);
    out.sy = fix16_to_int_round(sy_f);
    out.sz = cz2;  /* store raw fix16 depth (larger = farther) */
    out.visible = true;
    return out;
}
