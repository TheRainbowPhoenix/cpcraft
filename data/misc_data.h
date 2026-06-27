/* data/misc_data.h - Missing icon bitmap and swap helper */
#pragma once
#include "types.h"
#include "../src/tlsf.h"

const furnaceRecipe furnaceRecipes[furnaceRecipeAmount] = {
    {
        .inputItem = 10,
        .outputItem = 3,
    },
    {
        .inputItem = 12,
        .outputItem = 48,
    },
    {
        .inputItem = 13,
        .outputItem = 49,
    },
    {
        .inputItem = 7,
        .outputItem = 52,
    },
    {
        .inputItem = 59,
        .outputItem = 60,
    },
    {
        .inputItem = 98,
        .outputItem = 99,
    },
};
const craftingRecipe craftingRecipes[craftingRecipeAmount] = {
    {
        .needsCraftingTable = false, //planks

        .diffrentItemAmount = 1,
        .inputItem = {5, 0, 0, 0},
        .inputItemAmount = {1, 0, 0, 0},

        .outputItem = 8,
        .outputItemAmount = 4,
    },
    {
        .needsCraftingTable = false, //sticks

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {2, 0, 0, 0},

        .outputItem = 21,
        .outputItemAmount = 4,
    },

    {
        .needsCraftingTable = true, //woooden pickaxe

        .diffrentItemAmount = 2,
        .inputItem = {21, 8, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 22,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //stone pickaxe

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 23,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //iron pickaxe

        .diffrentItemAmount = 2,
        .inputItem = {21, 48, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 24,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //gold pickaxe

        .diffrentItemAmount = 2,
        .inputItem = {21, 49, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 25,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //diamond pickaxe

        .diffrentItemAmount = 2,
        .inputItem = {21, 51, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 26,
        .outputItemAmount = 1,
    },

    {
        .needsCraftingTable = true, //wooden axe

        .diffrentItemAmount = 2,
        .inputItem = {21, 8, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 27,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 28,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 48, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 29,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 49, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 30,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 51, 0, 0},
        .inputItemAmount = {2, 3, 0, 0},

        .outputItem = 31,
        .outputItemAmount = 1,
    },

    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 8, 0, 0},
        .inputItemAmount = {2, 1, 0, 0},

        .outputItem = 32,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {2, 1, 0, 0},

        .outputItem = 33,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 48, 0, 0},
        .inputItemAmount = {2, 1, 0, 0},

        .outputItem = 34,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 49, 0, 0},
        .inputItemAmount = {2, 1, 0, 0},

        .outputItem = 35,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 51, 0, 0},
        .inputItemAmount = {2, 1, 0, 0},

        .outputItem = 36,
        .outputItemAmount = 1,
    },

    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 8, 0, 0},
        .inputItemAmount = {1, 2, 0, 0},

        .outputItem = 37,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {1, 2, 0, 0},

        .outputItem = 38,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //

        .diffrentItemAmount = 2,
        .inputItem = {21, 48, 0, 0},
        .inputItemAmount = {1, 2, 0, 0},

        .outputItem = 39,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 49, 0, 0},
        .inputItemAmount = {1, 2, 0, 0},

        .outputItem = 40,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 51, 0, 0},
        .inputItemAmount = {1, 2, 0, 0},

        .outputItem = 41,
        .outputItemAmount = 1,
    },

    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 8, 0, 0},
        .inputItemAmount = {2, 2, 0, 0},

        .outputItem = 42,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {2, 2, 0, 0},

        .outputItem = 43,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 48, 0, 0},
        .inputItemAmount = {2, 2, 0, 0},

        .outputItem = 44,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 49, 0, 0},
        .inputItemAmount = {2, 2, 0, 0},

        .outputItem = 45,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true,

        .diffrentItemAmount = 2,
        .inputItem = {21, 51, 0, 0},
        .inputItemAmount = {2, 2, 0, 0},

        .outputItem = 46,
        .outputItemAmount = 1,
    },

    {
        .needsCraftingTable = false, //crafting table

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {4, 0, 0, 0},

        .outputItem = 19,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //furnace

        .diffrentItemAmount = 1,
        .inputItem = {10, 0, 0, 0},
        .inputItemAmount = {8, 0, 0, 0},

        .outputItem = 20,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = true, //chest

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {8, 0, 0, 0},

        .outputItem = 57,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = false, //bricks

        .diffrentItemAmount = 1,
        .inputItem = {60, 0, 0, 0},
        .inputItemAmount = {4, 0, 0, 0},

        .outputItem = 61,
        .outputItemAmount = 1,
    },
    {
        .needsCraftingTable = false, //torch

        .diffrentItemAmount = 2,
        .inputItem = {47, 21, 0, 0},
        .inputItemAmount = {1, 1, 0, 0},

        .outputItem = 62,
        .outputItemAmount = 4,
    },
    {
        .needsCraftingTable = true, //bread

        .diffrentItemAmount = 1,
        .inputItem = {66, 0, 0, 0},
        .inputItemAmount = {3, 0, 0, 0},

        .outputItem = 68,
        .outputItemAmount = 1,
    },

	{
        .needsCraftingTable = true, //wooden stairs

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 69,
        .outputItemAmount = 4,
    },
	{
        .needsCraftingTable = true, //wooden slab

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {3, 0, 0, 0},

        .outputItem = 73,
        .outputItemAmount = 6,
    },
	{
        .needsCraftingTable = true, //cobblestone stairs

        .diffrentItemAmount = 1,
        .inputItem = {10, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 70,
        .outputItemAmount = 4,
    },
	{
        .needsCraftingTable = true, //cobblestone slab

        .diffrentItemAmount = 1,
        .inputItem = {10, 0, 0, 0},
        .inputItemAmount = {3, 0, 0, 0},

        .outputItem = 74,
        .outputItemAmount = 6,
    },
	{
        .needsCraftingTable = true, //stone stairs

        .diffrentItemAmount = 1,
        .inputItem = {3, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 71,
        .outputItemAmount = 4,
    },
	{
        .needsCraftingTable = true, //stone slab

        .diffrentItemAmount = 1,
        .inputItem = {3, 0, 0, 0},
        .inputItemAmount = {3, 0, 0, 0},

        .outputItem = 75,
        .outputItemAmount = 6,
    },
	{
        .needsCraftingTable = true, //brick stairs

        .diffrentItemAmount = 1,
        .inputItem = {61, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 72,
        .outputItemAmount = 4,
    },
	{
        .needsCraftingTable = true, //brick slab

        .diffrentItemAmount = 1,
        .inputItem = {61, 0, 0, 0},
        .inputItemAmount = {3, 0, 0, 0},

        .outputItem = 76,
        .outputItemAmount = 6,
    },
	{
        .needsCraftingTable = true, //bed

        .diffrentItemAmount = 2,
        .inputItem = {8, 95, 0, 0},
        .inputItemAmount = {3, 3, 0, 0},

        .outputItem = 78,
        .outputItemAmount = 1,
    },

	{
        .needsCraftingTable = true, //wooden door

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 77,
        .outputItemAmount = 3,
    },
	{
        .needsCraftingTable = true, //iron door

        .diffrentItemAmount = 1,
        .inputItem = {48, 0, 0, 0},
        .inputItemAmount = {6, 0, 0, 0},

        .outputItem = 83,
        .outputItemAmount = 3,
    },

	{
        .needsCraftingTable = false, //redstone torch

        .diffrentItemAmount = 2,
        .inputItem = {21, 80, 0, 0},
        .inputItemAmount = {1, 1, 0, 0},

        .outputItem = 81,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //repeater

        .diffrentItemAmount = 3,
        .inputItem = {81, 80, 3, 0},
        .inputItemAmount = {2, 1, 3, 0},

        .outputItem = 82,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //redstone lamp

        .diffrentItemAmount = 2,
        .inputItem = {52, 80, 0, 0},
        .inputItemAmount = {8, 1, 0, 0},

        .outputItem = 84,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = false, //lever

        .diffrentItemAmount = 2,
        .inputItem = {21, 10, 0, 0},
        .inputItemAmount = {1, 1, 0, 0},

        .outputItem = 85,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = false, //wooden button

        .diffrentItemAmount = 1,
        .inputItem = {8, 0, 0, 0},
        .inputItemAmount = {1, 0, 0, 0},

        .outputItem = 86,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = false, //stone button

        .diffrentItemAmount = 1,
        .inputItem = {3, 0, 0, 0},
        .inputItemAmount = {1, 0, 0, 0},

        .outputItem = 87,
        .outputItemAmount = 1,
    },

	{
        .needsCraftingTable = true, //coal block //iron gold emerald diamond

        .diffrentItemAmount = 1,
        .inputItem = {47, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 88,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //iron block

        .diffrentItemAmount = 1,
        .inputItem = {48, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 89,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //gold block

        .diffrentItemAmount = 1,
        .inputItem = {49, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 90,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //diamond block

        .diffrentItemAmount = 1,
        .inputItem = {51, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 91,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //redstone block

        .diffrentItemAmount = 1,
        .inputItem = {80, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 92,
        .outputItemAmount = 1,
    },
	{
        .needsCraftingTable = true, //emerald block

        .diffrentItemAmount = 1,
        .inputItem = {50, 0, 0, 0},
        .inputItemAmount = {9, 0, 0, 0},

        .outputItem = 93,
        .outputItemAmount = 1,
    },
	{
		.needsCraftingTable = false, //shears

        .diffrentItemAmount = 1,
        .inputItem = {48, 0, 0, 0},
        .inputItemAmount = {2, 0, 0, 0},

        .outputItem = 94,
        .outputItemAmount = 1,
	},
};

Vector2S chunk2DPos[64];
bool chunk2DActive[64];
bool chunk2DReset[64];

int currentSlot = 0;
short hotbarBlockTypes[9] =  {-1, -1, -1, -1, -1, -1, -1, -1, -1};
short hotbarBlockAmount[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
short hotbarBlockHp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

short inventoryBlockTypes[27] =  {
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
short inventoryBlockAmount[27];
short inventoryBlockHp[27];

tlsf_t tlsf;

