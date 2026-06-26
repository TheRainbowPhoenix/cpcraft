/* src/engine/world.c */
/*
 * cpcraft-port — engine/world.c
 *
 * Implementation of the block-based voxel world.
 *
 * Terrain generation: a deterministic hash-based heightmap. For each
 * (x, z) column we compute a base height in [WORLD_H/4, WORLD_H/2] from
 * a hash of (x, z), then smooth it with a 3x3 neighborhood average.
 * Below the surface we fill stone, the top block becomes grass, and
 * we sprinkle in a few trees.
 *
 * This is intentionally simple — good enough to walk around in, not a
 * faithful port of CPCraft's Perlin-noise terrain.
 */
#include "world.h"
#include <string.h>

uint8_t world_blocks[WORLD_W * WORLD_H * WORLD_D];

bool block_is_transparent(uint8_t id)
{
    return id == BLK_AIR || id == BLK_LEAVES || id == BLK_GLASS || id == BLK_WATER;
}

bool block_is_solid(uint8_t id)
{
    return id != BLK_AIR && id != BLK_WATER;
}

bool block_is_fluid(uint8_t id)
{
    return id == BLK_WATER;
}

/* Deterministic hash of (x, z, seed) -> [0, 65535].
 *
 * Two multiplies + XORs. Cheap, no dependency on math.h. The constants
 * are picked to give a reasonable distribution for terrain. */
static uint32_t hash2d(int x, int z, uint32_t seed)
{
    uint32_t h = (uint32_t)x * 73856093u ^ (uint32_t)z * 19349663u ^ seed * 83492791u;
    h ^= h >> 13;
    h *= 0x85ebca6bu;
    h ^= h >> 16;
    return h;
}

/* Height at world column (x, z). Returns the Y of the top non-air block. */
static int column_height(int x, int z, uint32_t seed)
{
    /* Average a 3x3 neighborhood of hashes to smooth the terrain. */
    uint32_t sum = 0;
    for (int dz = -1; dz <= 1; dz++)
        for (int dx = -1; dx <= 1; dx++)
            sum += hash2d(x + dx, z + dz, seed) & 0xFFFF;
    sum /= 9;

    /* Map [0, 65535] -> [WORLD_H/4, WORLD_H*3/4]. */
    int h = (WORLD_H / 4) + (int)((sum * (WORLD_H / 2)) >> 16);
    if (h < 1) h = 1;
    if (h >= WORLD_H - 1) h = WORLD_H - 2;
    return h;
}

void world_generate(uint32_t seed)
{
    memset(world_blocks, BLK_AIR, sizeof(world_blocks));

    /* Fill terrain column by column. */
    for (int z = 0; z < WORLD_D; z++)
        for (int x = 0; x < WORLD_W; x++)
        {
            int h = column_height(x, z, seed);

            for (int y = 0; y <= h; y++)
            {
                uint8_t blk;
                if (y == h)        blk = BLK_GRASS;
                else if (y >= h-3) blk = BLK_DIRT;
                else               blk = BLK_STONE;
                world_set(x, y, z, blk);
            }

            /* Sprinkle a few trees on grass columns.
             * Tree probability: ~1 in 32 columns. */
            if ((hash2d(x, z, seed ^ 0xDEADBEEF) & 31) == 0 && h < WORLD_H - 6)
            {
                int trunk = 3 + (int)((hash2d(x, z, seed ^ 0xCAFEBABE) >> 4) & 3);
                for (int i = 1; i <= trunk; i++)
                    world_set(x, h + i, z, BLK_WOOD);

                int top = h + trunk;
                /* Leaf canopy: 5x5x3 blob centered on the trunk top. */
                for (int dy = -1; dy <= 1; dy++)
                    for (int dz = -2; dz <= 2; dz++)
                        for (int dx = -2; dx <= 2; dx++)
                        {
                            if (dx == 0 && dz == 0 && dy < 1) continue;
                            int lx = x + dx, ly = top + dy, lz = z + dz;
                            if ((unsigned)lx >= WORLD_W) continue;
                            if ((unsigned)lz >= WORLD_D) continue;
                            if ((unsigned)ly >= WORLD_H) continue;
                            if (world_get(lx, ly, lz) == BLK_AIR)
                                world_set(lx, ly, lz, BLK_LEAVES);
                        }
            }
        }
}

void world_init(void)
{
    world_generate(0);
}
