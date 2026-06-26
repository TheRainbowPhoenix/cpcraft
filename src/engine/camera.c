/* src/engine/camera.c */
/*
 * cpcraft-port — engine/camera.c
 *
 * 3D-to-2D projection implementation.
 *
 * The math is the standard yaw+pitch camera transform followed by a
 * perspective divide. We compute it in float (the SH-4A has soft-float
 * via libgcc; one projection per vertex is fine performance-wise).
 *
 * Steps:
 *   1. Translate world point by -eye.
 *   2. Rotate by -yaw around Y, then -pitch around X.
 *   3. The resulting (cx, cy, cz) is in camera space.
 *   4. If cz <= near, the point is behind the camera -> not visible.
 *   5. Project: sx = FB_W/2 + (cx / cz) * focal; sy = FB_H/2 - (cy / cz) * focal.
 *
 * The focal length controls the FOV. focal = FB_W / (2 * tan(hfov/2)).
 * For a 70° horizontal FOV: focal = 160 / (2 * tan(35°)) ≈ 114.
 */
#include "camera.h"
#include "framebuffer.h"
#include <math.h>

#define FOV_HORIZONTAL_DEG  70.0f
#define NEAR_PLANE          0.1f

ScreenPoint camera_project(float wx, float wy, float wz,
                           float eye_x, float eye_y, float eye_z,
                           float yaw_deg, float pitch_deg)
{
    ScreenPoint out = {0, 0, 0, false};

    /* Step 1: translate. */
    float dx = wx - eye_x;
    float dy = wy - eye_y;
    float dz = wz - eye_z;

    /* Step 2: rotate. Yaw rotates around Y, pitch around X.
     *
     * We rotate by -yaw (because rotating the world by -yaw is the same
     * as rotating the camera by +yaw).
     *
     * After yaw rotation (around Y, by -yaw):
     *   cx =  dx*cos(yaw) + dz*sin(yaw)
     *   cz = -dx*sin(yaw) + dz*cos(yaw)
     *   cy =  dy  (unchanged)
     *
     * Then pitch rotation (around X, by -pitch):
     *   cy' = cy*cos(pitch) - cz*sin(pitch)
     *   cz' = cy*sin(pitch) + cz*cos(pitch)
     *   cx' = cx  (unchanged)
     */
    const float DEG2RAD = 3.14159265f / 180.0f;
    float yr = yaw_deg   * DEG2RAD;
    float pr = pitch_deg * DEG2RAD;
    float cy_yaw = cosf(yr),  sy_yaw = sinf(yr);
    float cy_pit = cosf(pr),  sy_pit = sinf(pr);

    /* Yaw rotation (around Y). */
    float cx1 =  dx * cy_yaw + dz * sy_yaw;
    float cy1 =  dy;
    float cz1 = -dx * sy_yaw + dz * cy_yaw;

    /* Pitch rotation (around X). */
    float cx2 = cx1;
    float cy2 = cy1 * cy_pit - cz1 * sy_pit;
    float cz2 = cy1 * sy_pit + cz1 * cy_pit;

    /* Step 3: camera space is (cx2, cy2, cz2). In our convention +Z is
     * forward, so points in front of the camera have cz2 > 0. */
    if (cz2 <= NEAR_PLANE)
    {
        out.visible = false;
        return out;
    }

    /* Step 4: perspective project. Focal length derived from FOV.
     *
     * focal = (FB_W / 2) / tan(hfov / 2)
     * For 70° hfov: 80 / tan(35°) = 80 / 0.7002 ≈ 114. */
    const float focal = (FB_W * 0.5f) / tanf(FOV_HORIZONTAL_DEG * 0.5f * DEG2RAD);

    int sx = (int)(FB_W * 0.5f + (cx2 / cz2) * focal);
    int sy = (int)(FB_H * 0.5f - (cy2 / cz2) * focal);

    out.sx = sx;
    out.sy = sy;
    out.sz = cz2;
    out.visible = true;
    return out;
}
