/* src/engine/camera.c */
/*
 * cpcraft-port — engine/camera.c
 *
 * 3D-to-2D projection (int32_t, no floats, no struct return).
 */
#include "camera.h"
#include "framebuffer.h"
#include "math_lut.h"
#include "fix16.h"

#define NEAR_PLANE  6554    /* 0.1 in fix16 */

static fix16_t focal_length;

void camera_init(void)
{
    fix16_t s = fix16_sin_brads(deg_to_brads(35));
    fix16_t c = fix16_cos_brads(deg_to_brads(35));
    fix16_t tan35 = fix16_div(s, c);
    focal_length = fix16_div(fix16_from_int(FB_W / 2), tan35);
}

void camera_project(fix16_t wx, fix16_t wy, fix16_t wz,
                           fix16_t ex, fix16_t ey, fix16_t ez,
                    uint16_t yaw, int16_t pitch,
                    ScreenPoint *out)
{
    fix16_t dx = wx - ex;
    fix16_t dy = wy - ey;
    fix16_t dz = wz - ez;

    fix16_t cy = fix16_cos_brads(yaw);
    fix16_t sy = fix16_sin_brads(yaw);

    fix16_t cx1 = fix16_mul(dx, cy) + fix16_mul(dz, sy);
    fix16_t cy1 = dy;
    fix16_t cz1 = fix16_mul(-dx, sy) + fix16_mul(dz, cy);

    fix16_t cp = fix16_cos_brads((uint16_t)pitch);
    fix16_t sp = fix16_sin_brads((uint16_t)pitch);

    fix16_t cx2 = cx1;
    fix16_t cy2 = fix16_mul(cy1, cp) - fix16_mul(cz1, sp);
    fix16_t cz2 = fix16_mul(cy1, sp) + fix16_mul(cz1, cp);

    if (cz2 <= NEAR_PLANE) {
        out->visible = false;
        out->sx = 0; out->sy = 0; out->sz = 0;
        return;
    }

    fix16_t scale = fix16_div(FIX16_ONE, cz2);
    fix16_t sx_f = fix16_from_int(FB_W / 2) + fix16_mul(fix16_mul(cx2, scale), focal_length);
    fix16_t sy_f = fix16_from_int(FB_H / 2) - fix16_mul(fix16_mul(cy2, scale), focal_length);

    out->sx = fix16_to_int_round(sx_f);
    out->sy = fix16_to_int_round(sy_f);
    out->sz = cz2;
    out->visible = true;
}
