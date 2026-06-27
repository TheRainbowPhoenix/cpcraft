/* src/engine/camera.h */
/*
 * cpcraft-port — engine/camera.h
 *
 * 3D-to-2D projection (int32_t 16.16 fixed-point, no floats, no struct return).
 *
 * Coordinate system (right-handed):
 *   +X = east, +Y = up, +Z = south.
 *   Yaw 0 = looking down +Z, yaw 90° (BRAD 16384) = looking down +X.
 *   Pitch 0 = horizontal, pitch +89° = straight up.
 *
 * camera_project() writes to an output ScreenPoint pointer instead of
 * returning a struct. This avoids SH-4A struct-return alignment issues.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Screen-space point with depth. */
typedef struct {
    int sx, sy;         /* screen coords in framebuffer pixels */
    int32_t sz;         /* camera-space depth (16.16, >0 = in front) */
    bool visible;       /* false if behind the camera */
} ScreenPoint;

/* Initialize the camera (precompute focal length). Call once at startup. */
void camera_init(void);

/* Project a world-space point to screen space.
 *
 * Writes the result to *out (does NOT return a struct — avoids SH-4A
 * alignment issues with struct returns).
 *
 * All positions are int32_t (16.16 fixed-point). */
void camera_project(int32_t wx, int32_t wy, int32_t wz,
                    int32_t ex, int32_t ey, int32_t ez,
                    uint16_t yaw, int16_t pitch,
                    ScreenPoint *out);

#ifdef __cplusplus
}
#endif
