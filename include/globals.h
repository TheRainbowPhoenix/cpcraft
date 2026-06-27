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

int resXZBuffer = 192;
int resYZBuffer = 108;

int renderingMode; // 0=normal, 1=no transparent, 2=wireframe
int maxSpeed = 25;
// unsigned short ZBuffer[192*108];
unsigned short *ZBuffer;
float gravity = 9.81f;

float PPosX = 54;
float PPosY = 40;
float PPosZ = 54;

int startFallY = 0;

float velocityX;
float velocityY;
float velocityZ;

float rotationX = 90;
float rotationY = 0;

float playerMovementSpeed = 4;
float playerRotationSpeed = 90;

int partiLength = 1;
int usedEntitys = 0;
obj allObj[objLength];
particle allparticles[1];
entity entityList[entityLength];

chestData allchest[32];

int triangleLength;
int verticesLength;

short *VRAMAddress;
char *lightmap = 0x8C228800 + ALLOC_OFFSET;  // behind z-buffer
char *blockData = 0x8C282800 + ALLOC_OFFSET; // behind lightmap

float deltaTime;
float deltaTimeNoSlow;
int time1;

bool slow;

float cosTable[TABLE_SIZE];
float sinTable[TABLE_SIZE];

bool compressVertices = true;
bool stopChunkLoading = false;
bool caves = false;
bool OrePatches = false;
int advancedTerrain = 2;
bool renderUnderwater = true;
bool survival = false;
bool renderTextured = true;
bool lighting = true;
bool isOncCG50 = false;
bool mobs = false;
bool dayNightCycle = false;
bool isinfinite = false;
bool showPerformaceGraph = false;
bool beautifulSky = true;
char currentTexturePackIndex = 0;

int currentSelectedWorld = 0;
char worldExists[5] = {0, 0, 0, 0, 0};
int worldVersion[5] = {0, 0, 0, 0, 0};

int pixelSize = 2;
char skyBrightness = 8;

int blockSelected = 1;
int totalBlockAmount = 8;

unsigned short *screenColor = 0xE5200000;

float destructionTimer = 0;
float totalDestructionTimer = 0;
int destructionBlockIndex = 0;

int dayTime = 1200;

#define MULTIPLIER 1664525
#define INCREMENT 1013904223

/* Additional globals from the delta_time section */
int renderDistance = 0;
int redstoneRenderDistance = 1;

Vector3I removeBlock;
Vector3I addBlock;
bool isTopOfBlock = false;
int entityInFrontIndex;

/* Forward vector (computed by CalculateForwardVector) */
Vector3 forward;
/* World block data array */

/* Chunk management */
Vector2S chunk2DPos[64];
bool chunk2DActive[64];
bool chunk2DReset[64];

/* Hotbar / inventory */
int currentSlot = 0;
short hotbarBlockTypes[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
short hotbarBlockAmount[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
short hotbarBlockHp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
short inventoryBlockTypes[27] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
short inventoryBlockAmount[27];
short inventoryBlockHp[27];

/* Memory allocator */
tlsf_t tlsf;

/* UI state */
int UIState = 9;
bool ExitLoop = false;

/* Texture pointers */
color_t *textures2;
color_t *itemIcons2;
color_t *assetsInputBuffer;
color_t *randomShit;
color_t *screenColor;

/* World data */

/* Performance tracking */
int performanceTime1 = 0;
int performanceTime2 = 0;
float performanceTime = 0;
int fps = 0;

/* More missing globals */
int currentChunkUpdateTimer = 0;
int currentChunkUpdated = 0;
bool isInCraftingTable = false;
bool isInFurnace = false;
int currentChestIndex = 0;
int startColor = 0;
extern int SEED;

/* More globals */
int activeChunks = 0;
char dayTimeChange = 0;
unsigned char performace[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char Cperformace[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
color_t performanceColors[10] = {0xf800, 0xfc00, 0xffe0, 0xfc10, 0xfc1f,
                                 0xf81f, 0x841f, 0x041f, 0x001f, 0x07ff};
int renderdV = 0;
bool blocksRenderd[64];
