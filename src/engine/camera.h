/* src/engine/camera.h */
/*
 * cpcraft-port — engine/camera.h
 *
 * 3D-to-2D projection — PLAIN INT, no float, no fix16.
 *
 * Scale: 100 units per block. Sin/cos: 10000 = 1.0.
 *
 * Coordinate system (right-handed):
 *   +X = east, +Y = up, +Z = south.
 *   Yaw 0 = looking down +Z, yaw 90° (BRAD 16384) = looking down +X.
 *   Pitch 0 = horizontal, pitch +89° = straight up.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sx, sy;         /* screen coords in framebuffer pixels */
    int32_t sz;         /* camera-space depth (units, >0 = in front) */
    bool visible;
} ScreenPoint;

void camera_init(void);

/* Project a world-space point to screen space.
 * Positions are in units (100 per block). Writes to *out. */
void camera_project(int32_t wx, int32_t wy, int32_t wz,
                    int32_t ex, int32_t ey, int32_t ez,
                    uint16_t yaw, int16_t pitch,
                    ScreenPoint *out);

#ifdef __cplusplus
}
#endif
