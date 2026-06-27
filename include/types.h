/* include/types.h */
/* include/types.h
 * Type definitions extracted from eninge.c.
 * All vector types, game structs, and typedefs.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>



#define FILE_PATH "\\\\fls0\\File.txt"
#define M_PI 3.14159265358979323846
#define TABLE_SIZE 1024
#define TABLE_MASK (TABLE_SIZE - 1)
#define currentVersion 5

#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216
#define LCD_WIDTH_PX_HALF 192

typedef unsigned short color_t;
typedef struct {
    float x;
    float y;
} Vector2;
typedef struct {
    float x;
    float y;
    float z;
} Vector3;
typedef struct {
    float w;
    float x;
    float y;
    float z;
} Vector4;

typedef struct {
    int x;
    int y;
} Vector2I;
typedef struct {
    int x;
    int y;
    int z;
} Vector3I;
typedef struct {
    int w;
    int x;
    int y;
    int z;
} Vector4I;
typedef struct {
    char w;
    char x;
    char y;
    char z;
} Vector4B;

typedef struct {
    short x;
    short y;
} Vector2S;
typedef struct {
    unsigned short x;
    unsigned short y;
    unsigned short z;
} Vector3S;
typedef struct {
    unsigned short w;
    unsigned short x;
    unsigned short y;
    unsigned short z;
} Vector4S;

typedef struct {
    char x;
    char y;
    char z;
} Vector3B;

typedef struct {
    short x;
    short y;
    unsigned short z;
} screenPoint;

typedef struct {
    Vector3S *verticesIPC; //vertices int pre-calculated
    Vector4S *triangles;
    char *brightnes;
    unsigned char *color;
	color_t *solidColor;
    Vector3B *normal;
    volatile bool active;
    volatile int sizeV;  //size vertices
    volatile int sizeT;  //size triangles
    volatile Vector3 position;
    volatile Vector3I actualPosition;
    volatile Vector2 rotation;
    volatile unsigned char *textureSize;
} obj;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool gravity;
    float size;
    color_t color;
    char transparency;
    char mode;
    int maxTime;
    float time;
    bool used;
} particle;

typedef struct {
    volatile bool active;            //is entity active
    volatile unsigned char type;     //mob type
    volatile unsigned short state;    //state of the mob
	volatile unsigned short state2;    //state of the mob extra
	volatile short fallingSince;
    volatile float moveTimer;        //
    volatile unsigned short locationOnAllObj;
    volatile unsigned short health;
    volatile unsigned short extraEntity;
    volatile bool isFed;

    volatile float velocityX;
    volatile float velocityY;
    volatile float velocityZ;

    volatile bool renderTextured;

    Vector2 rotation;
	Vector3I position;

	bool hasFur;
} entity;

typedef struct{
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

typedef struct{

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
    int verticesLength;
    Vector3B vertices[10][16]; //768

    int diffrentRotationAmount;

    int facesLength;
    Vector4B faces[12]; //768

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

    unsigned char texureSize[12]; //96
    Vector3B dir[10][12]; //1152
    Vector3B normal[10][12]; //288
    char doSideCheck[12]; //96 - 3168+16 bytes per blocktype

    char otherBlocksRenderFace[10][12];
} blockType;

typedef struct {
    volatile bool active;
	
    volatile int start;
    volatile int end;
    volatile int size;

    volatile int **variable;
} customHeapBlock;

typedef struct {
    bool used;
    Vector3I position;

    short chestBlockTypes[27];
    short chestBlockAmount[27];
	short chestBlockHp[27];
} chestData;

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

