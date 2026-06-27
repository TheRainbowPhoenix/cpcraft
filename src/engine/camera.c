/* src/engine/camera.c */
/*
 * cpcraft-port — engine/camera.c
 *
 * 3D-to-2D projection — PLAIN INT math.
 *
 * Scale: 100 units per block. Sin/cos: 10000 = 1.0.
 *
 * Projection: sx = FB_W/2 + (cx * focal) / cz
 *             sy = FB_H/2 - (cy * focal) / cz
 * All plain int division. No fix16, no float, no int64 in hot paths.
 */
#include "camera.h"
#include "framebuffer.h"
#include "math_lut.h"
#include "fix16.h"

#define NEAR_PLANE  10    /* 0.1 blocks in units */

static int32_t focal_length;  /* in screen pixels (~114) */

void camera_init(void)
{
    /* focal = (FB_W / 2) / tan(35°)
     * tan(35°) = sin(35°) / cos(35°)
     * sin/cos return scale-10000 values. */
    int32_t s = sin_brads(deg_to_brads(35));  /* ~5736 */
    int32_t c = cos_brads(deg_to_brads(35));  /* ~8192 */
    /* tan = s * 10000 / c (plain int) */
    int32_t tan35 = s * TRIG_SCALE / c;  /* ~7002 */
    /* focal = (FB_W/2) * 10000 / tan35 */
    focal_length = (FB_W / 2) * TRIG_SCALE / tan35;  /* ~1142 */
    /* Actually we want focal in pixels, not scaled. Let's use:
     * focal_pixels = (FB_W/2) / tan(35°) = 80 / 0.7002 ≈ 114 */
    focal_length = 114;
}

void camera_project(int32_t wx, int32_t wy, int32_t wz,
                    int32_t ex, int32_t ey, int32_t ez,
                    uint16_t yaw, int16_t pitch,
                    ScreenPoint *out)
{
    /* Step 1: translate (units = 100 per block). */
    int32_t dx = wx - ex;
    int32_t dy = wy - ey;
    int32_t dz = wz - ez;

    /* Step 2: rotate by -yaw (around Y).
     * sin/cos return scale-10000. Multiply by dx/dz and divide by 10000. */
    int32_t cy = cos_brads(yaw);  /* 10000 = 1.0 */
    int32_t sy = sin_brads(yaw);

    /* cx1 =  dx*cy + dz*sy  (all /10000) */
    int32_t cx1 = (dx * cy + dz * sy) / TRIG_SCALE;
    int32_t cy1 = dy;
    int32_t cz1 = (-dx * sy + dz * cy) / TRIG_SCALE;

    /* Pitch rotation (around X). */
    int32_t cp = cos_brads((uint16_t)pitch);
    int32_t sp = sin_brads((uint16_t)pitch);

    int32_t cx2 = cx1;
    int32_t cy2 = (cy1 * cp - cz1 * sp) / TRIG_SCALE;
    int32_t cz2 = (cy1 * sp + cz1 * cp) / TRIG_SCALE;

    /* Step 3: behind-camera check. */
    if (cz2 <= NEAR_PLANE) {
        out->visible = false;
        out->sx = 0; out->sy = 0; out->sz = 0;
        return;
    }

    /* Step 4: perspective project — PLAIN INT DIVISION.
     *   sx = FB_W/2 + cx2 * focal / cz2
     *   sy = FB_H/2 - cy2 * focal / cz2
     * focal is in pixels (~114). cx2/cy2/cz2 are in units (100/block). */
    out->sx = FB_W / 2 + (cx2 * focal_length) / cz2;
    out->sy = FB_H / 2 - (cy2 * focal_length) / cz2;
    out->sz = cz2;
    out->visible = true;
}
