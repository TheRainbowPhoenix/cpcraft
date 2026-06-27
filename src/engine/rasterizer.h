/* src/engine/rasterizer.h */
/*
 * cpcraft-port — engine/rasterizer.h
 *
 * Z-buffered affine-textured quad rasterizer (PLAIN INT, no fix16).
 *
 * The rasterizer works entirely in screen space using plain int32_t.
 * No fix16 multiplication or division in the inner loop — just
 * integer addition. This is the fastest and safest approach for the
 * SH-4A (no alignment issues, no overflow, no 64-bit ops).
 *
 * Algorithm:
 *   1. Find min/max Y of the 4 projected vertices.
 *   2. For each scanline, find left/right X by intersecting edges.
 *   3. Walk the span using Bresenham-style accumulators for u, v, z.
 *
 * The caller provides ScreenPoint vertices (from camera_project).
 * The rasterizer reads sx, sy (screen coords), sz (camera-space depth),
 * and visible (behind-camera flag).
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"  /* for ScreenPoint typedef */

#ifdef __cplusplus
extern "C" {
#endif

/* Z-buffer. One uint16_t per framebuffer pixel. */
extern uint16_t rz_zbuf[FB_W * FB_H];

void rz_clear_zbuf(void);

void rz_draw_textured_quad(const ScreenPoint v[4],
                           const uint16_t tex[TEX_SIZE][TEX_SIZE]);

void rz_draw_solid_quad(const ScreenPoint v[4], uint16_t color);

#ifdef __cplusplus
}
#endif
