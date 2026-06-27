/* src/engine/fix16.h */
/*
 * cpcraft-port — engine/fix16.h
 *
 * PLAIN INT math — NO fixed-point, NO float, NO int64_t.
 *
 * Scale conventions:
 *   - World positions: 100 units per block (so 5.12 blocks = 512)
 *   - Sin/cos: 10000 = 1.0 (so sin(90°) = 10000)
 *   - Angles: BRAD (uint16_t, 0..65535 = 0..2π)
 *
 * This file is kept for backward compatibility — fix16_t is int32_t,
 * and the old constants are redefined in terms of the new scale.
 * But NEW code should use plain int32_t and the scale constants below.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Scale factors. */
#define BLOCK_SCALE     100     /* 1 block = 100 units */
#define TRIG_SCALE      10000   /* 1.0 = 10000 (for sin/cos) */

/* Convert between blocks and internal units. */
#define BLOCKS_TO_UNITS(b)   ((b) * BLOCK_SCALE)
#define UNITS_TO_BLOCKS(u)   ((u) / BLOCK_SCALE)

/* Backward compat: fix16_t is just int32_t. */
typedef int32_t fix16_t;

/* Old fix16 constants — mapped to new scale for compat.
 * WARNING: these don't match the old 16.16 scale! Only use if you
 * understand the new 100-units-per-block convention. */
#define FIX16_ONE       BLOCK_SCALE
#define FIX16_HALF      50
#define FIX16_TWO       200

#define fix16_from_int(v)      ((int32_t)((v) * BLOCK_SCALE))
#define fix16_to_int(v)        ((int)((v) / BLOCK_SCALE))
#define fix16_to_int_round(v)  ((int)(((v) + FIX16_HALF) / BLOCK_SCALE))
#define fix16_abs(v)           ((v) < 0 ? -(v) : (v))
#define fix16_frac(v)          ((v) % BLOCK_SCALE)

/* fix16_mul: multiply two trig-scale values (10000 = 1.0).
 * Result is also trig-scale. Uses plain int division. */
static inline int32_t fix16_mul(int32_t a, int32_t b) {
    return (a * b) / TRIG_SCALE;
}

/* fix16_div: divide two trig-scale values. */
static inline int32_t fix16_div(int32_t a, int32_t b) {
    if (b == 0) return 0;
    return ((int64_t)a * TRIG_SCALE) / b;  /* one int64 multiply is OK here — not in hot loops */
}

#define fix16_recip(a)  fix16_div(TRIG_SCALE, (a))

static inline int32_t fix16_lerp(int32_t a, int32_t b, int32_t t) {
    return a + (b - a) * t / TRIG_SCALE;
}

/* Old sin/cos compat wrappers — call the new scale-10000 versions. */
#define fix16_sin_brads(a)  sin_brads(a)
#define fix16_cos_brads(a)  cos_brads(a)

#ifdef __cplusplus
}
#endif
