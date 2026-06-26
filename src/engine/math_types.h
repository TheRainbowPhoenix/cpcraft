/* src/engine/math_types.h */
/*
 * cpcraft-port — engine/math_types.h
 *
 * Integer-only 3D vector types and helpers.
 *
 * These mirror the layout of CPCraft's Vector2I/Vector3I/Vector2S/etc.
 * so we can lift code from cpcraft-dev/src/eninge.c with minimal
 * restructuring.
 *
 * All arithmetic is integer (no floats / FPU) for two reasons:
 *   1. The ClassPad's SH-4A has no FPU, so float ops are slow.
 *   2. The engine's projection math (pointToScreenCorrdinates) was
 *      originally written with fixed-point intermediates (* 1000),
 *      so integer math is the natural fit.
 *
 * For trig we use the lookup tables in math_lut.h.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 2D integer vector — used for screen-space coords and chunk positions. */
typedef struct { int x, y; } Vec2i;

/* 3D integer vector — used for world/block positions. */
typedef struct { int x, y, z; } Vec3i;

/* 4-component signed-byte vector — used by CPCraft for cube face indices
 * (4 corner indices packed into 4 bytes). */
typedef struct { int8_t w, x, y, z; } Vec4b;

/* 2D signed-short vector — used by CPCraft for screen-space coords in the
 * triangle rasterizer (16-bit z-buffer). */
typedef struct { int16_t x, y; } Vec2s;

/* Fixed-point 3D vector. We use 16.16 fixed point (1.0 = 65536) for
 * world positions and directions in the projection math. */
typedef struct { int32_t x, y, z; } Vec3fxp;

/* Helpers. */
static inline Vec3i vec3i_make(int x, int y, int z) {
    Vec3i v = {x, y, z}; return v;
}
static inline Vec3i vec3i_add(Vec3i a, Vec3i b) {
    Vec3i v = {a.x + b.x, a.y + b.y, a.z + b.z}; return v;
}
static inline Vec3i vec3i_sub(Vec3i a, Vec3i b) {
    Vec3i v = {a.x - b.x, a.y - b.y, a.z - b.z}; return v;
}
static inline int vec3i_eq(Vec3i a, Vec3i b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

#ifdef __cplusplus
}
#endif
