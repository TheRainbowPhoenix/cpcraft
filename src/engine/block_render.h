/* src/engine/block_render.h */
/*
 * cpcraft-port — engine/block_render.h
 *
 * Render the voxel world by iterating visible blocks and drawing each
 * exposed face as a textured quad through the rasterizer.
 *
 * Algorithm:
 *   1. Compute the camera's eye + yaw/pitch.
 *   2. Walk every block in the world (WORLD_W × WORLD_H × WORLD_D).
 *   3. For each non-air block, check its 6 neighbors. For each neighbor
 *      that is air or transparent, the corresponding face is "exposed"
 *      and should be drawn.
 *   4. For each exposed face, compute the 4 world-space corners (in
 *      counter-clockwise order), project them through the camera, and
 *      call rz_draw_textured_quad() with the face's texture.
 *
 * Face culling:
 *   We do basic backface culling by computing the face's normal in
 *   world space and dotting it with the view direction. If the dot
 *   product is positive, the face is pointing away from the camera
 *   and we skip it.
 *
 * Texture selection:
 *   Each block ID has 6 face textures (top, bottom, +X, -X, +Z, -Z).
 *   For now we use a single texture per block (top texture for top,
 *   side texture for sides, bottom for bottom). The texture table is
 *   in block_render.c — block_textures[id] gives the texture to use
 *   for sides; top_textures[id] for the top face.
 *
 * This is a brute-force renderer: O(world volume) per frame. For a
 * 64×32×64 world that's 131 072 block iterations, of which maybe
 * 5% have exposed faces. Good enough for a prototype; a real port
 * would use chunk meshing + frustum culling.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "texture.h"
#include "world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Texture table. block_textures[id] is the texture used for the block's
 * side faces; top_textures[id] for the top face; bottom_textures[id]
 * for the bottom. Initialized by block_render_init(). */
extern const uint16_t (*block_textures[BLK_COUNT])[TEX_SIZE];
extern const uint16_t (*top_textures[BLK_COUNT])[TEX_SIZE];
extern const uint16_t (*bottom_textures[BLK_COUNT])[TEX_SIZE];

/* Initialize the texture table (assigns procedural textures from
 * texture.h to each block ID). Call after tex_init(). */
void block_render_init(void);

/* Render the entire world from the player's current viewpoint.
 *
 * Uses the global player state (player.x/y/z, player.yaw/pitch) for the
 * camera. Walks every block in the world and draws exposed faces.
 *
 * This is the main world-draw call. The caller should:
 *   1. fb_clear(sky_color)
 *   2. rz_clear_zbuf()
 *   3. world_render()
 *   4. fb_present() */
void world_render(void);

#ifdef __cplusplus
}
#endif
