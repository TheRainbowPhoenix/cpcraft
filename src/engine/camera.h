/* src/engine/camera.h */
/*
 * cpcraft-port — engine/camera.h
 *
 * 3D-to-2D projection.
 *
 * Ported from CPCraft's pointToScreenCorrdinates() in eninge.c, but
 * adapted to:
 *   - Use floats (CPCraft used fixed-point * 1000, but we don't have
 *     the FPU vs. soft-float constraints here since this only runs
 *     once per vertex, not per pixel).
 *   - Target our 160×264 framebuffer (FB_W × FB_H) instead of CPCraft's
 *     320×180. The projection scales accordingly.
 *
 * Coordinate system (matches CPCraft / right-handed):
 *   +X = east, +Y = up, +Z = south.
 *   Yaw 0 = looking down +Z, yaw 90 = looking down +X.
 *   Pitch 0 = horizontal, pitch +89 = straight up.
 *
 * The camera is at the player's eye position. We project a world-space
 * point to screen-space (sx, sy) and a depth value (z), where z > 0
 * means "in front of the camera". If z <= 0 the point is behind the
 * camera and the caller should skip it.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Screen-space point with depth. sx/sy are in framebuffer pixels
 * (0..FB_W-1, 0..FB_H-1). sz is the camera-space depth (>0 = in front). */
typedef struct {
    int sx, sy;
    float sz;
    bool visible;   /* false if behind the camera or outside the frustum */
} ScreenPoint;

/* Project a world-space point (wx, wy, wz) to screen space.
 *
 * Uses the global player's eye position + yaw/pitch. The caller passes
 * the eye + angles in to avoid a hidden dependency on player.h. */
ScreenPoint camera_project(float wx, float wy, float wz,
                           float eye_x, float eye_y, float eye_z,
                           float yaw_deg, float pitch_deg);

#ifdef __cplusplus
}
#endif
