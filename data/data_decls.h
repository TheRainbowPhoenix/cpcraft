/* data/data_decls.h */
/* data/data_decls.h
 * Extern declarations for all data tables.
 * Include this from engine.h — it declares the tables without defining them.
 * The actual definitions are in src/data_tables.c.
 */
#pragma once
#include "types.h"

#ifndef itemAmount
#define itemAmount 100
#endif
#ifndef textureBlockAmount
#define textureBlockAmount 136
#endif
#ifndef craftingRecipeAmount
#define craftingRecipeAmount 57
#endif
#ifndef furnaceRecipeAmount
#define furnaceRecipeAmount 6
#endif
#ifndef chestAmount
#define chestAmount 32
#endif
#ifndef entityLength
#define entityLength 128
#endif
#ifndef objLength
#define objLength 228
#endif

/* Sky */
extern const unsigned short skyColors[16];
extern const int skyboxColors[16][128];

/* Icons */
extern const color_t itemIcons[itemAmount][676];
extern const color_t minercaftLogo[9702];
extern const color_t buttons[2][3200];
extern color_t missingIcon[676];
extern color_t missingTexture[256];

/* Font */
extern const color_t asciiMap[16384];
extern const char asciiLength[256];

/* UI elements */
extern const color_t recipeSelector[12800];
extern const color_t noCraftingTableCross[676];
extern const color_t hotbarDeselected[900];
extern const color_t hotbarSelected[1024];
extern const color_t heart[2][196];
extern const color_t inventory_palette[7];
extern const color_t chestInv_palette[7];
extern const color_t allColors[42];

/* Block/item/blocktype definitions */
extern const blockType blockTypes[19];
extern const item allItem[itemAmount];
extern const block allBlock[73];

/* Recipes */
extern const craftingRecipe craftingRecipes[craftingRecipeAmount];
extern const furnaceRecipe furnaceRecipes[furnaceRecipeAmount];

/* Texture type */
extern int textureType[textureBlockAmount];
extern const unsigned char asciiMap2[2048];
