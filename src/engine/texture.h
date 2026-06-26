/* src/engine/texture.h */
/*
 * cpcraft-port — engine/texture.h
 *
 * Tiny indexed-texture module.
 *
 * CPCraft stores block textures as 16x16 RGB565 images, packed one per face.
 * We mirror that layout so future port work can drop in CPCraft's texture
 * data unchanged.
 *
 * For now we only provide:
 *   - A simple procedural texture (checkerboard) so the demo has something
 *     to render without shipping external assets.
 *   - fb_textured_rect() — draw a textured axis-aligned rectangle with
 *     nearest-neighbor sampling. This is the building block for both the
 *     floor caster and the wall caster we'll add later when porting cpcraft.
 */
#pragma once

#include <stdint.h>
#include "framebuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEX_SIZE 16   /* 16x16 textures, RGB565 */

/* Procedural checkerboard texture (16x16). Generated at startup. */
extern uint16_t tex_checker[TEX_SIZE][TEX_SIZE];

/* Procedural "stone" texture — same size, looks noisy/blocky. */
extern uint16_t tex_stone[TEX_SIZE][TEX_SIZE];

/* Procedural "grass" texture — green with darker speckles. */
extern uint16_t tex_grass[TEX_SIZE][TEX_SIZE];

/* Initialize the procedural textures. */
void tex_init(void);

/* Draw a textured rectangle into the virtual framebuffer.
 *
 *   (x, y)    top-left corner in the virtual framebuffer
 *   (w, h)    size in the virtual framebuffer
 *   tex       pointer to a TEX_SIZE x TEX_SIZE RGB565 texture
 *   u_offset  horizontal texture offset (for animation), in pixels, [0, TEX_SIZE)
 *   v_offset  vertical texture offset, in pixels, [0, TEX_SIZE)
 *
 * Uses nearest-neighbor sampling. Clips to the framebuffer.
 *
 * This is the simplest textured-rect draw: it samples (u, v) = ((sx * TEX_SIZE) / w, (sy * TEX_SIZE) / h).
 * For raycaster-style perspective-correct texturing we'll add a separate
 * fb_textured_vline() helper later. */
void fb_textured_rect(int x, int y, int w, int h,
                      const uint16_t tex[TEX_SIZE][TEX_SIZE],
                      int u_offset, int v_offset);

#ifdef __cplusplus
}
#endif
