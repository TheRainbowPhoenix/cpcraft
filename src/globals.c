/* src/globals.c */
/* src/globals.c - All global variable definitions in one translation unit. */
#include "engine.h"

int resXZBuffer = 192;
int resYZBuffer = 108;
int renderingMode = 0;
int maxSpeed = 25;
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
int renderDistance = 0;
int redstoneRenderDistance = 1;
Vector3I removeBlock;
Vector3I addBlock;
bool isTopOfBlock = false;
int entityInFrontIndex;
Vector3 forward;
Vector2S chunk2DPos[64];
bool chunk2DActive[64];
bool chunk2DReset[64];
int currentSlot = 0;
short hotbarBlockTypes[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
short hotbarBlockAmount[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
short hotbarBlockHp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
short inventoryBlockAmount[27];
short inventoryBlockHp[27];
tlsf_t tlsf;
int UIState = 9;
bool ExitLoop = false;
int performanceTime1 = 0;
int performanceTime2 = 0;
float performanceTime = 0;
int fps = 0;
int currentChunkUpdateTimer = 0;
int currentChunkUpdated = 0;
bool isInCraftingTable = false;
bool isInFurnace = false;
int currentChestIndex = 0;
int startColor = 0;
int activeChunks = 0;
char dayTimeChange = 0;
unsigned char performace[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char Cperformace[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
color_t performanceColors[10] = {0xf800, 0xfc00, 0xffe0, 0xfc10, 0xfc1f,
                                 0xf81f, 0x841f, 0x041f, 0x001f, 0x07ff};
int renderdV = 0;
#include "chunk_constants.h"
bool blocksRenderd[TOTALCHUNKWIDTH * TOTALCHUNKWIDTH];

/* Global texture pointers (initialized to memory-mapped addresses) */
color_t *textures2 = (color_t *)(0x8C540000 + ALLOC_OFFSET);
color_t *itemIcons2 = (color_t *)(0x8C580000 + ALLOC_OFFSET);
color_t *assetsInputBuffer = (color_t *)(0x8C500000 + ALLOC_OFFSET);
color_t *randomShit = (color_t *)(0x8C5B0000 + ALLOC_OFFSET);

/* Also add missing globals that were in sky_colors.h */
int health = 20;

/* SEED for noise */
int SEED = 0;

/* Missing definitions that were skipped by the extraction script */
short *VRAMAddress;
char *lightmap = (char *)(0x8C228800 + ALLOC_OFFSET);
char *blockData = (char *)(0x8C282800 + ALLOC_OFFSET);
Vector3I removeBlock;
Vector3I addBlock;
Vector3 forward;
Vector2S chunk2DPos[64];
short inventoryBlockTypes[27] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
