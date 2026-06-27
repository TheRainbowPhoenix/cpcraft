/* include/globals.h */
/* include/globals.h
 * Global variables extracted from eninge.c.
 * Player position, rendering state, world config, etc.
 */
#pragma once

#include "types.h"

void swap(Vector2I** a, Vector2I** b) {
    Vector2I* temp = *a;
    *a = *b;
    *b = temp;
}

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

int renderingMode;          //0=normal, 1=no transparent, 2=wireframe
int maxSpeed = 25;
//unsigned short ZBuffer[192*108];
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
char *lightmap = 0x8C228800 + ALLOC_OFFSET; //behind z-buffer
char *blockData = 0x8C282800 + ALLOC_OFFSET; //behind lightmap

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
