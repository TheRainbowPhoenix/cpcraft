/* src/engine/game_data.h */
/*
 * cpcraft-port — engine/game_data.h
 *
 * CPCraft's data structures, ported from cpcraft-dev/src/eninge.c.
 *
 * These are the "game logic" classes that CPCraft uses:
 *   - Item: defines an item type (block, tool, food, etc.)
 *   - Block: defines a block type's properties (destroy time, drops, etc.)
 *   - Entity: a mob or item drop in the world (sheep, pig, dropped item)
 *   - Particle: a visual effect (explosion, block break)
 *   - ChestData: a chest's inventory contents
 *   - CraftingRecipe / FurnaceRecipe: crafting/smelting recipes
 *
 * All floats replaced with int32_t. All arrays sized down for the
 * ClassPad's limited RAM.
 *
 * This is a "ready for use" header — the data tables are defined in
 * game_data.c and the engine can look them up by index.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "fix16.h"
#include "math_types.h"
#include "world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 *  Items
 * ========================================================================
 *
 * Ported from CPCraft's `item` struct. Each item has a name, stack size,
 * tool properties, and either a block ID (if it's a block item) or an
 * icon index (if it's a non-block item like a pickaxe).
 *
 * CPCraft has 100 items; we keep the same indices for compatibility. */
#define ITEM_COUNT  100

typedef struct {
    char name[20];
    char stackSize;       /* max stack size (64 for most blocks) */
    short breaksIn;       /* tool durability (-1 if not a tool) */
    bool isBlock;         /* true if this item places a block */
    uint8_t blockId;      /* which block to place (if isBlock) */
    char toolType;        /* 0=none, 1=pickaxe, 2=axe, 3=shovel, 4=sword */
    char toolLevel;       /* 0=wood, 1=stone, 2=iron, 3=gold, 4=diamond */
    short burnTime;       /* fuel value in ticks (for furnace) */
    short textureIfNoBlock; /* icon index for non-block items */
    uint8_t damage;       /* attack damage */
    short iconIndex;      /* icon index in the itemIcons array */
} Item;

extern const Item item_table[ITEM_COUNT];

/* ========================================================================
 *  Block definitions
 * ========================================================================
 *
 * Ported from CPCraft's `block` struct. Defines per-block properties:
 * destroy time per tool type, texture indices, drops, etc. */
#define BLOCK_DEF_COUNT  19

typedef struct {
    short itemIndex;          /* which item this block drops when broken */
    short fullBlockIndex;     /* item index for the full block form */
    int32_t destroyTime[6];   /* time to destroy with each tool level (0=hand) */
    uint8_t textureIndex[10]; /* texture indices for each face */
    short drops;              /* item index of the drop (-1 if none) */
    char toolToBreak;         /* minimum tool type needed */
    char toolLevelToGetItem;  /* minimum tool level to get the drop */
    uint8_t blockType;        /* 0=full, 1=transparent */
    char brightness;          /* light emission (0=none) */
} BlockDef;

extern const BlockDef block_def_table[BLOCK_DEF_COUNT];

/* ========================================================================
 *  Entities (mobs + item drops)
 * ========================================================================
 *
 * Ported from CPCraft's `entity` struct. Entities are mobs (sheep, pig)
 * or item drops. Each entity has a position, velocity, health, and AI
 * state.
 *
 * CPCraft uses floats for position/velocity; we use int32_t.
 *
 * Entity types:
 *   1 = sheep
 *   2 = pig
 *   3 = item drop
 */
#define ENTITY_COUNT  32

enum EntityType {
    ENTITY_NONE = 0,
    ENTITY_SHEEP = 1,
    ENTITY_PIG = 2,
    ENTITY_ITEM = 3,
};

enum EntityState {
    ENT_STANDING = 0,
    ENT_WALKING = 1,
    ENT_GRAZING = 2,
    ENT_FOLLOWING = 3,
    ENT_RUNNING = 4,
    ENT_HIT = 5,
};

typedef struct {
    bool active;
    uint8_t type;           /* EntityType */
    uint16_t state;         /* EntityState */
    uint16_t state2;        /* extra state data */
    int16_t fallingSince;   /* how long the entity has been falling */
    int32_t moveTimer;      /* AI timer */
    uint16_t health;
    uint16_t extraEntity;   /* for sheep: the "sheared" version entity index */
    bool isFed;

    int32_t vx, vy, vz;     /* velocity (fix16 per frame) */
    bool renderTextured;

    /* Position (in block coordinates, fix16). */
    int32_t x, y, z;

    bool hasFur;            /* sheep: has wool? */
} Entity;

extern Entity entity_list[ENTITY_COUNT];

/* Find a free entity slot and return its index, or -1 if full. */
int entity_alloc(void);

/* Spawn an item drop at (x, y, z) with the given item index. */
void entity_spawn_item(int32_t x, int32_t y, int32_t z, int itemIndex);

/* Spawn a sheep at (x, y, z). */
void entity_spawn_sheep(int32_t x, int32_t y, int32_t z);

/* Spawn a pig at (x, y, z). */
void entity_spawn_pig(int32_t x, int32_t y, int32_t z);

/* Update all entities (AI, physics, collisions). Call once per frame. */
void entity_update_all(int32_t dt);

/* Clear all entities. */
void entity_clear_all(void);

/* ========================================================================
 *  Particles
 * ========================================================================
 *
 * Visual effects: block break debris, explosions, etc. */
#define PARTICLE_COUNT  64

typedef struct {
    bool active;
    int32_t x, y, z;       /* position */
    int32_t vx, vy, vz;    /* velocity */
    int32_t size;
    uint16_t color;
    uint8_t transparency;
    uint8_t mode;
    int maxTime;
    int32_t time;
    bool gravity;
} Particle;

extern Particle particle_list[PARTICLE_COUNT];

/* Spawn a particle. */
void particle_spawn(int32_t x, int32_t y, int32_t z,
                    int32_t vx, int32_t vy, int32_t vz,
                    int32_t size, uint16_t color,
                    int maxTime, bool gravity);

/* Update all particles. Call once per frame. */
void particle_update_all(int32_t dt);

/* ========================================================================
 *  Chest data
 * ========================================================================
 *
 * A chest has 27 slots. Each slot has an item index and count. */
#define CHEST_COUNT  16
#define CHEST_SLOTS  27

typedef struct {
    bool used;
    int x, y, z;    /* chest position in the world */
    short item[CHEST_SLOTS];
    short count[CHEST_SLOTS];
    short hp[CHEST_SLOTS];  /* tool durability */
} ChestData;

extern ChestData chest_list[CHEST_COUNT];

/* Find or create a chest at (x, y, z). Returns the chest index. */
int chest_get_or_create(int x, int y, int z);

/* ========================================================================
 *  Crafting / smelting recipes
 * ========================================================================
 *
 * Ported from CPCraft's craftingRecipe and furnaceRecipe structs. */
#define CRAFTING_RECIPE_COUNT  57
#define FURNACE_RECIPE_COUNT   6

typedef struct {
    bool needsCraftingTable;
    char ingredientCount;
    short inputItem[4];
    short inputAmount[4];
    short outputItem;
    char outputAmount;
} CraftingRecipe;

typedef struct {
    short inputItem;
    short outputItem;
} FurnaceRecipe;

extern const CraftingRecipe crafting_recipes[CRAFTING_RECIPE_COUNT];
extern const FurnaceRecipe furnace_recipes[FURNACE_RECIPE_COUNT];

/* Initialize all game data (item table, block defs, recipes, entity
 * arrays). Call once at startup. */
void game_data_init(void);

#ifdef __cplusplus
}
#endif
