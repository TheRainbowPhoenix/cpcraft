/* src/engine/camera.h */
/*
 * cpcraft-port — engine/camera.h
 *
 * 3D-to-2D projection (16.16 fixed-point, no floats).
 *
 * Coordinate system (right-handed):
 *   +X = east, +Y = up, +Z = south.
 *   Yaw 0 = looking down +Z, yaw 90° (BRAD 16384) = looking down +X.
 *   Pitch 0 = horizontal, pitch +89° = straight up.
 *
 * The camera is at the player's eye position. We project a world-space
 * point to screen-space (sx, sy) and a depth value (sz), where sz > 0
 * means "in front of the camera". If sz <= 0 the point is behind the
 * camera and the caller should skip it.
 *
 * sz is stored as fix16_t (raw camera-space z). Larger = farther.
 * The rasterizer converts this to 1/z for the z-buffer test.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "fix16.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sx, sy;         /* screen coords in framebuffer pixels */
    fix16_t sz;         /* camera-space depth (fix16, >0 = in front) */
    bool visible;       /* false if behind the camera */
} ScreenPoint;

/* Initialize the camera (precompute focal length from FOV). Call once
 * at startup, after engine_init(). */
void camera_init(void);

/* Project a world-space point (wx, wy, wz) to screen space.
 *
 * eye + yaw + pitch come from the player. All positions are fix16_t. */
ScreenPoint camera_project(fix16_t wx, fix16_t wy, fix16_t wz,
                           fix16_t ex, fix16_t ey, fix16_t ez,
                           uint16_t yaw, int16_t pitch);

#ifdef __cplusplus
}
#endif
