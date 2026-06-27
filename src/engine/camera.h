/* src/engine/camera.h */
/*
 * cpcraft-port — engine/camera.h
 *
 * 3D-to-2D projection (int32_t 16.16 fixed-point, no floats, no struct return).
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "fix16.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sx, sy;
    fix16_t sz;
    bool visible;
} ScreenPoint;

void camera_init(void);

/* Project a world-space point to screen space.
 * Writes the result to *out (does NOT return a struct — avoids SH-4A
 * alignment issues with struct returns). */
void camera_project(fix16_t wx, fix16_t wy, fix16_t wz,
                           fix16_t ex, fix16_t ey, fix16_t ez,
                    uint16_t yaw, int16_t pitch,
                    ScreenPoint *out);

#ifdef __cplusplus
}
#endif
