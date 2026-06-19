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

typedef struct {
    unsigned char verticesLength;
    unsigned char facesLength;
    bool canWalkThrough;
} blockType;

typedef struct {
    short iconIndex;
    char stackSize;
    char toolLevel;
    char toolType;
    char breaksIn;
    char blockId;
} item;

typedef struct {
    char transparency;
    char id;
    bool hasInventory;
    char destroyTime[5];
    char blockType;
    char drops;
} block;

typedef struct {
    int x, y, z;
} screenPoint;

typedef struct {
    void* verticesIPC;
    void* triangles;
    void* normal;
    void* color;
    void* brightnes;
    void* textureSize;
    void* solidColor;
    Vector3I position;
} obj;

typedef struct {
    bool mode;
} particle;

typedef struct {
    float moveTimer;
} entity;

typedef struct {
    unsigned char data[16*16*128];
} chunk;

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
