#ifndef ENINGE_H
#define ENINGE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

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
    int x;
    int y;
    int z;
    int w;
} Vector4I;

typedef struct {
    int x;
    int y;
} Vector2I;

typedef struct {
    unsigned char verticesLength;
    unsigned char facesLength;
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
    int x;
    int y;
} screenPoint;

#endif
