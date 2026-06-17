#ifndef ENINGE_H
#define ENINGE_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
typedef unsigned short color_t;
typedef struct { float x, y; } Vector2;
typedef struct { float x, y, z; } Vector3;
typedef struct { int x, y, z, w; } Vector4I;
typedef struct { int x, y; } Vector2I;
typedef struct { unsigned char verticesLength, facesLength; } blockType;
typedef struct { short iconIndex; char stackSize, toolLevel, toolType, breaksIn, blockId; } item;
typedef struct { char transparency, id; bool hasInventory; char destroyTime[5]; char blockType, drops; } block;
typedef struct { int x, y; } screenPoint;
#endif
