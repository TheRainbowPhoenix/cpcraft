/* src/engine/world.h */
/*
 * cpcraft-port — engine/world.h
 *
 * Block-based voxel world.
 *
 * Design (simplified from CPCraft):
 *   - The world is a fixed grid of CHUNKS_W × CHUNKS_D chunks.
 *   - Each chunk is CHUNK_W × CHUNK_H × CHUNK_D blocks.
 *   - Each block is one byte — a block ID (0 = air, 1 = stone, 2 = grass,
 *     3 = dirt, 4 = wood, 5 = leaves, 6 = sand, 7 = water, etc.).
 *
 * We use a flat array of CHUNKS_W*CHUNK_W * CHUNK_H * CHUNKS_D*CHUNK_D
 * bytes — small enough to fit in the ClassPad's heap (a 4×4×1 chunk
 * grid of 16³ blocks = 65 536 bytes = 64 KiB).
 *
 * Terrain generation is a simple heightmap-based generator: per (x, z)
 * column we compute a height from a deterministic hash + smoothing,
 * then fill stone below, dirt just below the surface, grass on top.
 * Trees are sprinkled in via a per-column hash.
 *
 * This is intentionally a stub — good enough to walk around in, not
 * trying to match CPCraft's Perlin-noise terrain.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Chunk dimensions. CPCraft uses 12×40×12; we use 16×16×16 for power-of-2
 * alignment (lets the compiler use shifts instead of multiplies). */
#define CHUNK_W   16
#define CHUNK_H   32
#define CHUNK_D   16

/* How many chunks in each horizontal direction. Total world size is
 * CHUNKS_W*CHUNK_W × CHUNKS_D*CHUNK_D blocks horizontally and
 * CHUNK_H blocks vertically. */
#define CHUNKS_W  4
#define CHUNKS_D  4

/* Derived world bounds. */
#define WORLD_W   (CHUNKS_W * CHUNK_W)   /* 64 */
#define WORLD_H   CHUNK_H                /* 32 */
#define WORLD_D   (CHUNKS_D * CHUNK_D)   /* 64 */

/* Block IDs. Keep these stable — the renderer and texture mapper both
 * index arrays by block ID. */
enum BlockId {
    BLK_AIR    = 0,
    BLK_STONE  = 1,
    BLK_GRASS  = 2,
    BLK_DIRT   = 3,
    BLK_WOOD   = 4,
    BLK_LEAVES = 5,
    BLK_SAND   = 6,
    BLK_WATER  = 7,
    BLK_COBBLE = 8,
    BLK_PLANK  = 9,
    BLK_GLASS  = 10,
    BLK_COUNT
};

/* True if the block is air or transparent (renderer should skip drawing
 * the faces between two transparent blocks). */
bool block_is_transparent(uint8_t id);

/* True if the block is solid (player collides with it). */
bool block_is_solid(uint8_t id);

/* True if the block is a fluid (water/lava). */
bool block_is_fluid(uint8_t id);

/* The world's block storage. Indexed as [y][z][x] so horizontal slices
 * are contiguous in memory (helps the renderer scan a Y layer).
 *
 * Total size: WORLD_W * WORLD_H * WORLD_D = 64*32*64 = 131 072 bytes = 128 KiB.
 * That's bigger than we'd like for the ClassPad heap, but acceptable for
 * a prototype. A real port would chunk this and only keep nearby chunks
 * resident. */
extern uint8_t world_blocks[WORLD_W * WORLD_H * WORLD_D];

/* Index a block at (x, y, z). Returns BLK_AIR for out-of-range coords
 * (so the renderer can treat the world as surrounded by air, and the
 * player can walk off the edge without crashing). */
static inline uint8_t world_get(int x, int y, int z)
{
    if ((unsigned)x >= WORLD_W || (unsigned)y >= WORLD_H || (unsigned)z >= WORLD_D)
        return BLK_AIR;
    return world_blocks[(y * WORLD_D + z) * WORLD_W + x];
}

/* Set a block at (x, y, z). No-op for out-of-range coords. */
static inline void world_set(int x, int y, int z, uint8_t id)
{
    if ((unsigned)x >= WORLD_W || (unsigned)y >= WORLD_H || (unsigned)z >= WORLD_D)
        return;
    world_blocks[(y * WORLD_D + z) * WORLD_W + x] = id;
}

/* Generate the world's terrain. Called once at startup. The seed picks
 * which deterministic heightmap we use. */
void world_generate(uint32_t seed);

/* Initialize the world module (zeros the array, then calls world_generate
 * with seed 0). */
void world_init(void);

#ifdef __cplusplus
}
#endif
