/* include/globals.h */
/* include/globals.h
 * Global variables extracted from eninge.c.
 * Player position, rendering state, world config, etc.
 */
#pragma once

#include "../src/tlsf.h"
#include "types.h"

void swap(Vector2I **a, Vector2I **b);

#define resX 384
#define resY 216

#define itemAmount 100
#define textureBlockAmount 136
#define craftingRecipeAmount 57
#define furnaceRecipeAmount 6
#define chestAmount 32
#define entityLength 128
#define objLength 228

extern int resXZBuffer;
extern int resYZBuffer;

int renderingMode; // 0=normal, 1=no transparent, 2=wireframe
extern int maxSpeed;
// unsigned short ZBuffer[192*108];
extern unsigned short *ZBuffer;
extern float gravity;

extern float PPosX;
extern float PPosY;
extern float PPosZ;

extern int startFallY;

extern float velocityX;
extern float velocityY;
extern float velocityZ;

extern float rotationX;
extern float rotationY;

extern float playerMovementSpeed;
extern float playerRotationSpeed;

extern int partiLength;
extern int usedEntitys;
extern obj allObj[objLength];
extern particle allparticles[1];
extern entity entityList[entityLength];

extern chestData allchest[32];

extern int triangleLength;
extern int verticesLength;

short *VRAMAddress;
extern char *lightmap;  // behind z-buffer
extern char *blockData; // behind lightmap

extern float deltaTime;
extern float deltaTimeNoSlow;
extern int time1;

extern bool slow;

extern float cosTable[TABLE_SIZE];
extern float sinTable[TABLE_SIZE];

extern bool compressVertices;
extern bool stopChunkLoading;
extern bool caves;
extern bool OrePatches;
extern int advancedTerrain;
extern bool renderUnderwater;
extern bool survival;
extern bool renderTextured;
extern bool lighting;
extern bool isOncCG50;
extern bool mobs;
extern bool dayNightCycle;
extern bool isinfinite;
extern bool showPerformaceGraph;
extern bool beautifulSky;
extern char currentTexturePackIndex;

extern int currentSelectedWorld;
extern char worldExists[5];
extern int worldVersion[5];

extern int pixelSize;
extern char skyBrightness;

extern int blockSelected;
extern int totalBlockAmount;

extern unsigned short *screenColor;

extern float destructionTimer;
extern float totalDestructionTimer;
extern int destructionBlockIndex;

extern int dayTime;

#define MULTIPLIER 1664525
#define INCREMENT 1013904223

/* Additional globals from the delta_time section */
extern int renderDistance;
extern int redstoneRenderDistance;

extern Vector3I removeBlock;
extern Vector3I addBlock;
extern bool isTopOfBlock;
extern int entityInFrontIndex;

/* Forward vector (computed by CalculateForwardVector) */
extern Vector3 forward;
/* World block data array */

/* Chunk management */
extern Vector2S chunk2DPos[64];
extern bool chunk2DActive[64];
extern bool chunk2DReset[64];

/* Hotbar / inventory */
extern int currentSlot;
extern short hotbarBlockTypes[9];
extern short hotbarBlockAmount[9];
extern short hotbarBlockHp[9];
extern short inventoryBlockTypes[27];
extern short inventoryBlockAmount[27];
extern short inventoryBlockHp[27];

/* Memory allocator */
extern tlsf_t tlsf;

/* UI state */
extern int UIState;
extern bool ExitLoop;

/* Texture pointers */
color_t *textures2;
color_t *itemIcons2;
color_t *assetsInputBuffer;
color_t *randomShit;
color_t *screenColor;

/* World data */

/* Performance tracking */
extern int performanceTime1;
extern int performanceTime2;
extern float performanceTime;
extern int fps;

/* More missing globals */
extern int currentChunkUpdateTimer;
extern int currentChunkUpdated;
extern bool isInCraftingTable;
extern bool isInFurnace;
extern int currentChestIndex;
extern int startColor;
extern int SEED;

/* More globals */
extern int activeChunks;
extern char dayTimeChange;
extern unsigned char performace[10];
extern unsigned char Cperformace[10];
color_t performanceColors[10] = {0xf800, 0xfc00, 0xffe0, 0xfc10, 0xfc1f,
                                 0xf81f, 0x841f, 0x041f, 0x001f, 0x07ff};
extern int renderdV;
extern bool blocksRenderd[64];
