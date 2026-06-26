/* src/engine/game_data.c */
/*
 * cpcraft-port — engine/game_data.c
 *
 * Ported data tables + entity/particle/chest management from CPCraft.
 *
 * All floats replaced with fix16_t. All arrays sized for the ClassPad's
 * limited RAM.
 */
#include "game_data.h"
#include "math_lut.h"
#include <string.h>

/* ========================================================================
 *  Item table — ported from CPCraft's allItem[100]
 * ========================================================================
 *
 * Each entry mirrors CPCraft's `item` struct. We keep the same indices
 * (0=air, 1=grass, 2=dirt, ...) for compatibility.
 *
 * Only the first ~30 items are fully defined; the rest are stubs that
 * can be filled in as the port progresses. */
const Item item_table[ITEM_COUNT] = {
    /* 0: air */
    {"air",        64, -1, true,  BLK_AIR,   0, 0, 0, 0, 0, 0},
    /* 1: grass block */
    {"grass",      64, -1, true,  BLK_GRASS, 0, 0, 0, 0, 1, 1},
    /* 2: dirt */
    {"dirt",       64, -1, true,  BLK_DIRT,  0, 0, 0, 0, 1, 2},
    /* 3: stone */
    {"stone",      64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 3},
    /* 4: leaves */
    {"leaves",     64, -1, true,  BLK_LEAVES,0, 0, 0, 0, 1, 4},
    /* 5: wood log */
    {"log",        64, -1, true,  BLK_WOOD,  0, 0, 0, 0, 1, 5},
    /* 6: water */
    {"water",      64, -1, true,  BLK_WATER, 0, 0, 0, 0, 0, 6},
    /* 7: sand */
    {"sand",       64, -1, true,  BLK_SAND,  0, 0, 0, 0, 1, 7},
    /* 8: planks */
    {"plank",      64, -1, true,  BLK_PLANK, 0, 0, 0, 0, 1, 8},
    /* 9: bedrock */
    {"bedrock",    64, -1, true,  BLK_STONE, 0, 0, 0, 0, 0, 9},
    /* 10: cobblestone */
    {"cobblestone",64, -1, true,  BLK_COBBLE,0, 0, 0, 0, 1, 10},
    /* 11: coal ore */
    {"coal ore",   64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 11},
    /* 12: iron ore */
    {"iron ore",   64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 12},
    /* 13: gold ore */
    {"gold ore",   64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 13},
    /* 14: diamond ore */
    {"diamond ore",64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 14},
    /* 15: glowstone */
    {"glowstone",  64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 15},
    /* 16: TNT */
    {"tnt",        64, -1, true,  BLK_STONE, 0, 0, 0, 0, 1, 16},
    /* 17: crafting table */
    {"craft table",64, -1, true,  BLK_PLANK, 0, 0, 0, 0, 1, 17},
    /* 18: furnace */
    {"furnace",    64, -1, true,  BLK_COBBLE,0, 0, 0, 0, 1, 18},
    /* 19: stick */
    {"stick",      64, -1, false, 0, 0, 0, 0, 0, 1, 19},
    /* 20: wooden pickaxe */
    {"w pickaxe",  1,  60, false, 0, 1, 0, 0, 0, 2, 20},
    /* 21: stone pickaxe */
    {"s pickaxe",  1, 132, false, 0, 1, 1, 0, 0, 3, 21},
    /* 22: iron pickaxe */
    {"i pickaxe",  1, 251, false, 0, 1, 2, 0, 0, 4, 22},
    /* 23: gold pickaxe */
    {"g pickaxe",  1,  33, false, 0, 1, 3, 0, 0, 2, 23},
    /* 24: diamond pickaxe */
    {"d pickaxe",  1,1562, false, 0, 1, 4, 0, 0, 5, 24},
    /* 25: wooden axe */
    {"w axe",      1,  60, false, 0, 2, 0, 0, 0, 2, 25},
    /* 26: stone axe */
    {"s axe",      1, 132, false, 0, 2, 1, 0, 0, 3, 26},
    /* 27: iron axe */
    {"i axe",      1, 251, false, 0, 2, 2, 0, 0, 4, 27},
    /* 28: gold axe */
    {"g axe",      1,  33, false, 0, 2, 3, 0, 0, 2, 28},
    /* 29: diamond axe */
    {"d axe",      1,1562, false, 0, 2, 4, 0, 0, 5, 29},
    /* 30-99: stubs */
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
    {"",0,-1,false,0,0,0,0,0,0,0},
};

/* ========================================================================
 *  Block definitions — stubs for now (CPCraft has 19 block types)
 * ======================================================================== */
const BlockDef block_def_table[BLOCK_DEF_COUNT] = {
    /* 0: air-like (placeholder) */
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 1, 0},
    /* 1: grass */
    {1, 1, {FIX16_ONE, FIX16_HALF, FIX16_HALF, 0,0,0}, {2,2,2,3,3,3}, 1, 0, 0, 0, 0},
    /* 2: dirt */
    {2, 2, {FIX16_HALF, FIX16_HALF, 0, 0,0,0}, {3,3,3,3,3,3}, 2, 0, 0, 0, 0},
    /* 3: stone */
    {3, 3, {FIX16_ONE*3, FIX16_ONE*2, FIX16_ONE, 0,0,0}, {4,4,4,4,4,4}, 3, 1, 1, 0, 0},
    /* 4: leaves */
    {4, 4, {FIX16_HALF, 0, 0, 0,0,0}, {5,5,5,5,5,5}, 4, 0, 0, 1, 0},
    /* 5: wood log */
    {5, 5, {FIX16_ONE*2, FIX16_ONE, 0, 0,0,0}, {6,6,6,7,7,7}, 5, 0, 0, 0, 0},
    /* 6: water */
    {6, 6, {0,0,0,0,0,0}, {8,8,8,8,8,8}, -1, 0, 0, 1, 0},
    /* 7: sand */
    {7, 7, {FIX16_HALF, FIX16_HALF, 0, 0,0,0}, {9,9,9,9,9,9}, 7, 0, 0, 0, 0},
    /* 8: planks */
    {8, 8, {FIX16_ONE, FIX16_HALF, 0, 0,0,0}, {10,10,10,10,10,10}, 8, 0, 0, 0, 0},
    /* 9-18: stubs */
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
    {0, 0, {0,0,0,0,0,0}, {0}, -1, 0, 0, 0, 0},
};

/* ========================================================================
 *  Entity management
 * ======================================================================== */

Entity entity_list[ENTITY_COUNT];

int entity_alloc(void)
{
    for (int i = 0; i < ENTITY_COUNT; i++)
        if (!entity_list[i].active)
            return i;
    return -1;
}

void entity_spawn_item(fix16_t x, fix16_t y, fix16_t z, int itemIndex)
{
    int slot = entity_alloc();
    if (slot < 0) return;

    memset(&entity_list[slot], 0, sizeof(Entity));
    entity_list[slot].active = true;
    entity_list[slot].type = ENTITY_ITEM;
    entity_list[slot].state = itemIndex;
    entity_list[slot].health = 1;
    entity_list[slot].x = x;
    entity_list[slot].y = y;
    entity_list[slot].z = z;
    entity_list[slot].renderTextured = true;
}

void entity_spawn_sheep(fix16_t x, fix16_t y, fix16_t z)
{
    int slot = entity_alloc();
    if (slot < 0) return;

    memset(&entity_list[slot], 0, sizeof(Entity));
    entity_list[slot].active = true;
    entity_list[slot].type = ENTITY_SHEEP;
    entity_list[slot].state = ENT_STANDING;
    entity_list[slot].health = 8;  /* sheep have 4 hearts = 8 HP */
    entity_list[slot].x = x;
    entity_list[slot].y = y;
    entity_list[slot].z = z;
    entity_list[slot].hasFur = true;
    entity_list[slot].renderTextured = true;
    entity_list[slot].moveTimer = 0;
}

void entity_spawn_pig(fix16_t x, fix16_t y, fix16_t z)
{
    int slot = entity_alloc();
    if (slot < 0) return;

    memset(&entity_list[slot], 0, sizeof(Entity));
    entity_list[slot].active = true;
    entity_list[slot].type = ENTITY_PIG;
    entity_list[slot].state = ENT_STANDING;
    entity_list[slot].health = 10;  /* pigs have 5 hearts = 10 HP */
    entity_list[slot].x = x;
    entity_list[slot].y = y;
    entity_list[slot].z = z;
    entity_list[slot].renderTextured = true;
    entity_list[slot].moveTimer = 0;
}

/* Simple entity AI + physics. */
void entity_update_all(fix16_t dt)
{
    for (int i = 0; i < ENTITY_COUNT; i++)
    {
        Entity *e = &entity_list[i];
        if (!e->active) continue;

        /* Gravity. */
        e->vy -= 1456;  /* GRAVITY_PER_FRAME */
        if (e->vy < -54613) e->vy = -54613;

        /* Apply velocity. */
        e->x += e->vx;
        e->z += e->vz;
        e->y += e->vy;

        /* Simple ground collision: if below the terrain, stop. */
        int bx = fix16_to_int(e->x);
        int bz = fix16_to_int(e->z);
        int by = fix16_to_int(e->y);
        if (by < 0) { e->y = 0; e->vy = 0; }

        /* Check block below. */
        uint8_t below = world_get(bx, by - 1, bz);
        if (block_is_solid(below) && e->vy < 0) {
            e->y = fix16_from_int(by);
            e->vy = 0;
        }

        /* AI per type. */
        if (e->type == ENTITY_SHEEP || e->type == ENTITY_PIG)
        {
            e->moveTimer -= dt;

            if (e->moveTimer <= 0)
            {
                /* Random walk: pick a new direction. */
                uint32_t h = (uint32_t)(i * 7919 + (int)e->x * 73856093) ^
                             (uint32_t)((int)e->z * 19349663);
                h ^= h >> 13;
                int action = h & 7;

                if (action < 4) {
                    /* Walk in a random direction. */
                    e->state = ENT_WALKING;
                    fix16_t angle = (h >> 4) & 0xFFFF;  /* random BRAD */
                    fix16_t speed = 1966;  /* ~0.03 blocks/frame */
                    e->vx = fix16_mul(fix16_sin_brads(angle), speed);
                    e->vz = fix16_mul(fix16_cos_brads(angle), speed);
                    e->moveTimer = fix16_from_int(2 + ((h >> 8) & 3));
                } else {
                    /* Stand still. */
                    e->state = ENT_STANDING;
                    e->vx = 0;
                    e->vz = 0;
                    e->moveTimer = fix16_from_int(2 + ((h >> 8) & 3));
                }
            }

            /* If hit, run away. */
            if (e->state == ENT_HIT) {
                e->vx = -e->vx;
                e->vz = -e->vz;
                e->state = ENT_RUNNING;
                e->moveTimer = fix16_from_int(3);
            }
        }
        else if (e->type == ENTITY_ITEM)
        {
            /* Item drops slow down. */
            e->vx = fix16_mul(e->vx, 0x60000);  /* 0.75 friction */
            e->vz = fix16_mul(e->vz, 0x60000);
        }
    }
}

void entity_clear_all(void)
{
    memset(entity_list, 0, sizeof(entity_list));
}

/* ========================================================================
 *  Particles
 * ======================================================================== */

Particle particle_list[PARTICLE_COUNT];

void particle_spawn(fix16_t x, fix16_t y, fix16_t z,
                    fix16_t vx, fix16_t vy, fix16_t vz,
                    fix16_t size, uint16_t color,
                    int maxTime, bool gravity)
{
    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        if (!particle_list[i].active)
        {
            particle_list[i].active = true;
            particle_list[i].x = x; particle_list[i].y = y; particle_list[i].z = z;
            particle_list[i].vx = vx; particle_list[i].vy = vy; particle_list[i].vz = vz;
            particle_list[i].size = size;
            particle_list[i].color = color;
            particle_list[i].maxTime = maxTime;
            particle_list[i].time = 0;
            particle_list[i].gravity = gravity;
            return;
        }
    }
}

void particle_update_all(fix16_t dt)
{
    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        Particle *p = &particle_list[i];
        if (!p->active) continue;

        p->x += p->vx;
        p->y += p->vy;
        p->z += p->vz;

        if (p->gravity)
            p->vy -= 1456;

        p->time += dt;
        if (p->time >= fix16_from_int(p->maxTime))
            p->active = false;
    }
}

/* ========================================================================
 *  Chest data
 * ======================================================================== */

ChestData chest_list[CHEST_COUNT];

int chest_get_or_create(int x, int y, int z)
{
    /* Find existing chest at this position. */
    for (int i = 0; i < CHEST_COUNT; i++)
        if (chest_list[i].used && chest_list[i].x == x &&
            chest_list[i].y == y && chest_list[i].z == z)
            return i;

    /* Create a new one. */
    for (int i = 0; i < CHEST_COUNT; i++)
    {
        if (!chest_list[i].used)
        {
            memset(&chest_list[i], 0, sizeof(ChestData));
            chest_list[i].used = true;
            chest_list[i].x = x;
            chest_list[i].y = y;
            chest_list[i].z = z;
            return i;
        }
    }
    return -1;  /* no free chest slot */
}

/* ========================================================================
 *  Crafting / smelting recipes
 * ======================================================================== */

const CraftingRecipe crafting_recipes[CRAFTING_RECIPE_COUNT] = {
    /* 0: planks from wood (1 log → 4 planks) */
    {false, 1, {5, 0, 0, 0}, {1, 0, 0, 0}, 8, 4},
    /* 1: sticks from planks (2 planks → 4 sticks) */
    {false, 1, {8, 8, 0, 0}, {1, 1, 0, 0}, 19, 4},
    /* 2: crafting table (4 planks → 1 table) */
    {false, 4, {8, 8, 8, 8}, {1, 1, 1, 1}, 17, 1},
    /* 3: furnace (8 cobblestone → 1 furnace) */
    {true, 8, {10,10,10,10}, {1,1,1,1}, 18, 1},
    /* 4: wooden pickaxe (3 planks + 2 sticks) */
    {true, 2, {8, 19, 0, 0}, {3, 2, 0, 0}, 20, 1},
    /* 5: stone pickaxe (3 stone + 2 sticks) */
    {true, 2, {3, 19, 0, 0}, {3, 2, 0, 0}, 21, 1},
    /* 6: iron pickaxe (3 iron + 2 sticks) — requires iron ingot (stub) */
    {true, 2, {3, 19, 0, 0}, {3, 2, 0, 0}, 22, 1},
    /* 7-56: stubs */
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
    {false, 0, {0,0,0,0}, {0,0,0,0}, 0, 0},
};

const FurnaceRecipe furnace_recipes[FURNACE_RECIPE_COUNT] = {
    /* 0: iron ore → iron ingot (stub index) */
    {12, 30},
    /* 1: gold ore → gold ingot (stub index) */
    {13, 31},
    /* 2: sand → glass (stub index) */
    {7, 32},
    /* 3: cobblestone → stone */
    {10, 3},
    /* 4: raw pork → cooked pork (stub) */
    {95, 96},
    /* 5: clay → brick (stub) */
    {0, 0},
};

/* ========================================================================
 *  Init
 * ======================================================================== */

void game_data_init(void)
{
    entity_clear_all();
    memset(particle_list, 0, sizeof(particle_list));
    memset(chest_list, 0, sizeof(chest_list));
}
