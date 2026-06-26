/* src/engine/raycast.h */
/*
 * cpcraft-port — engine/raycast.h
 *
 * DDA voxel raycaster — casts rays through the voxel grid and renders
 * textured vertical strips. This is the CP-Raycaster-Demo approach,
 * adapted for our 3D voxel world.
 *
 * Why raycasting instead of triangle/quad rasterization?
 *   1. Much simpler math — no projection of 4 corners per face.
 *   2. Much faster — one ray per screen column (160 rays) vs walking
 *      every block in the world (131072 iterations).
 *   3. Proven on the ClassPad — CP-Raycaster-Demo uses exactly this.
 *   4. No z-buffer needed — the DDA guarantees front-to-back ordering.
 *
 * Algorithm (per screen column x):
 *   1. Compute the ray direction from camera yaw + FOV.
 *   2. DDA march through the voxel grid in the XZ plane.
 *   3. For each step, check if the column at (ix, iz) has any non-air
 *      block from top to bottom.
 *   4. When we hit a solid column, find the highest block and draw a
 *      textured vertical strip for it.
 *   5. Fill above with sky color, below with floor color.
 *
 * This is a 2.5D renderer — it casts rays in the XZ plane and renders
 * walls as vertical strips. Floor and ceiling are solid colors for now.
 * A future version could add floor/ceiling texture casting.
 */
#pragma once

#include <stdint.h>
#include "fix16.h"
#include "framebuffer.h"
#include "texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Render the world using DDA raycasting.
 *
 * Uses the global player state (position, yaw, pitch) for the camera.
 * The caller should fb_clear(sky_color) and then call this.
 *
 * Parameters:
 *   eye_x/y/z  — camera position (fix16)
 *   yaw        — BRAD angle (0 = +Z, 16384 = +X)
 *   pitch      — BRAD angle (0 = horizontal, +16384 = up)
 *
 * The raycaster renders into fb_vram (the 160x264 virtual framebuffer).
 * Each screen column x gets one ray. */
void raycast_render(fix16_t eye_x, fix16_t eye_y, fix16_t eye_z,
                    uint16_t yaw, int16_t pitch);

#ifdef __cplusplus
}
#endif
