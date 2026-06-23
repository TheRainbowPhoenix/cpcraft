#ifndef ENINGE_H
#define ENINGE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned short color_t;

typedef struct { float x, y; } Vector2;
typedef struct { float x, y, z; } Vector3;
typedef struct { float w, x, y, z; } Vector4;
typedef struct { int x, y; } Vector2I;
typedef struct { int x, y, z; } Vector3I;
typedef struct { int x, y, z, w; } Vector4I;
typedef struct { signed char x, y, z; } Vector3B;
typedef struct { short x, y, z; } Vector3S;
typedef struct { short x, y; } Vector2S;
typedef struct { signed char x, y, z, w; } Vector4B;
typedef struct { short x, y, z, w; } Vector4S;

typedef struct {
    unsigned char verticesLength;
    Vector3B vertices[16][16];
    unsigned char diffrentRotationAmount;
    unsigned char facesLength;
    Vector4B faces[16];
    bool isFullBlock;
    bool isTransprent;
    bool isfluid;
    bool lightGetsThroughP1;
    bool lightGetsThroughP1Half;
    bool lightGetsThroughP2;
    bool canWalkThrough;
    bool hasToBeOnGrass;
    bool hasToBeOnBlock;
    bool canTransferRedstoneSignal;
    bool canHoldRedstoneSignal;
    bool canHaveGrassBelow;
    unsigned char texureSize[16];
    Vector3B dir[16][16];
    Vector3B normal[16][16];
    unsigned char doSideCheck[16];
    unsigned char otherBlocksRenderFace[16][6];
} blockType;

typedef struct {
    char name[20];
    char stackSize;
    short breaksIn; //how long the tool will last set to -1 if no tool
    bool isBlock;
    unsigned char blockId;
    char toolType;
    char toolLevel;
    short burnTime; //in ticks
    short textureIfNoBlock; //item that returns when item is put into furnace
    unsigned char damage;
    short iconIndex;
} item;

typedef struct {
    short itemIndex;
    short fullBlockIndex;
    float destroyTime[6];
    unsigned char noTextureColorIndex[2][10];
    unsigned char textureIndex[10];
    short drops;
    char toolToBeak;
    char toolLevelToGetItem;
    unsigned char blockType; //0=full, 1=transparent
    char brightness;
} block;

typedef struct {
    int x, y, z;
} screenPoint;

typedef struct {
    Vector3 position;
    Vector3I actualPosition;
    int sizeV;
    int sizeT;
    Vector3S* verticesIPC;
    Vector2 rotation;
    Vector4S* triangles;
    Vector3B* normal;
    unsigned char* color;
    unsigned char* brightnes;
    unsigned char* textureSize;
    color_t* solidColor;
    bool active;
} obj;

typedef struct {
    bool mode;
    color_t color;
} particle;

typedef struct {
    float moveTimer;
    float velocityY;
    int state2;
    int locationOnAllObj;
    bool active;
    int type;
    bool renderTextured;
    int extraEntity;
} entity;

typedef struct {
    unsigned char data[16*16*128];
} chunk;

typedef struct
{
    short inputItem;
    short outputItem;
} furnaceRecipe;

typedef struct
{
    bool needsCraftingTable;
    char diffrentItemAmount;
    short inputItem[4];
    short inputItemAmount[4];
    short outputItem;
    char outputItemAmount;
} craftingRecipe;

typedef struct{
	Vector4I UV1;
	Vector4I UV2;
	Vector4I UV3;
	Vector4I UV4;
} quadUV;

typedef struct
{
  unsigned short id, type;
  unsigned long fsize, dsize;
  unsigned int property;
  unsigned long address;
} file_type_t;

typedef struct {
    bool used;
    Vector3I position;
    short chestBlockTypes[27];
    short chestBlockAmount[27];
    short chestBlockHp[27];
} chestData;

extern const unsigned short skyColors[16];
extern const int skyboxColors[16][128];
extern const color_t minercaftLogo[9702];
extern const color_t buttons[2][3200];
extern const color_t asciiMap[16384];
extern const unsigned char asciiMap2[2048];
extern const char asciiLength[256];
extern const color_t recipeSelector[12800];
extern const color_t noCraftingTableCross[676];
extern const color_t hotbarDeselected[900];
extern const color_t hotbarSelected[1024];
extern const color_t heart[2][196];
extern const color_t inventory_palette[7];
extern const unsigned char inventory[17688];
extern const color_t chestInv_palette[7];
extern const unsigned char chestInv[28944];
extern const color_t allColors[42];
extern const blockType blockTypes[19];
extern const item allItem[100];
extern const block allBlock[73];

#endif
