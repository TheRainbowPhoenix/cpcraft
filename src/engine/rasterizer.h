/* src/engine/rasterizer.h */
/*
 * cpcraft-port — engine/rasterizer.h
 *
 * Z-buffered affine-textured quad rasterizer.
 *
 * Why quads not triangles?
 *   Every visible face of a voxel cube is a perfect square in world space.
 *   When projected through the camera it becomes a convex quad. We can
 *   rasterize that quad directly with a span-based filler, no need to
 *   split into two triangles. This halves the per-face setup cost and
 *   avoids the affine-warp seam down the diagonal that triangle
 *   rasterizers produce.
 *
 * Z-buffer:
 *   We keep a 16-bit z-buffer at FB_W × FB_H resolution (one entry per
 *   framebuffer pixel). Each entry stores a 1/cz value (so closer =
 *   larger), quantized to 16 bits. The rasterizer rejects pixels whose
 *   1/cz is smaller than the buffer's value.
 *
 * Affine texture mapping:
 *   We interpolate (u, v) linearly across the quad in screen space.
 *   This produces visible warping on large oblique faces but it's the
 *   cheapest option and matches CPCraft's approach. A perspective-correct
 *   version would require interpolating u/cz, v/cz, 1/cz and dividing
 *   per pixel — too slow for the ClassPad.
 *
 * The textured-rect helper from texture.h is used for UI (which doesn't
 * need the z-buffer). This module is for world geometry.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "framebuffer.h"
#include "camera.h"
#include "texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Z-buffer. One uint16_t per framebuffer pixel. Stores a quantized
 * 1/cz value (larger = closer). Initialized to 0 (farthest). */
extern uint16_t rz_zbuf[FB_W * FB_H];

/* Initialize the z-buffer (call at the start of each frame, before any
 * draw_face calls). Sets every entry to 0 (= "infinitely far"). */
void rz_clear_zbuf(void);

/* Draw a textured quad.
 *
 *   v[0..3]   — the 4 projected screen-space corners of the quad, in
 *                clockwise or counter-clockwise order (must be convex).
 *   tex       — pointer to a TEX_SIZE × TEX_SIZE RGB565 texture.
 *
 * The quad is rasterized as a series of horizontal spans. For each span
 * we interpolate u, v, and 1/cz linearly between the left and right
 * edges, fetch a texture pixel, test against the z-buffer, and write.
 *
 * Faces with all 4 vertices behind the camera are skipped. Faces with
 * some vertices behind the camera are NOT clipped (we just skip them
 * for simplicity — at 70° FOV and the player's position constraints,
 * this rarely happens, and when it does the missing face is usually
 * invisible anyway because it's grazing the camera). */
void rz_draw_textured_quad(const ScreenPoint v[4],
                           const uint16_t tex[TEX_SIZE][TEX_SIZE]);

/* Draw a solid-color quad (used for untextured fallback / water tinting). */
void rz_draw_solid_quad(const ScreenPoint v[4], uint16_t color);

#ifdef __cplusplus
}
#endif
