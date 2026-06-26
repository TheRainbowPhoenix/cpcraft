/* src/engine/fix16.h */
/*
 * cpcraft-port — engine/fix16.h
 *
 * 16.16 fixed-point arithmetic.
 *
 * The ClassPad's SH-4A has NO FPU. Float operations are emulated in
 * software (libgcc), which is both slow AND produces subtly different
 * results vs. real IEEE 754. For a real-time game we need deterministic,
 * fast math — 16.16 fixed-point gives us that.
 *
 * Type: fix16_t = int32_t, with 16 integer bits and 16 fractional bits.
 *   Range: [-32768.0, 32767.99998]
 *   Resolution: 1/65536 ≈ 0.0000153
 *
 * For our voxel world (64×32×64 blocks), positions fit comfortably in
 * the integer part. Player velocity, rotation deltas, and projection
 * intermediates all fit in the fractional part.
 *
 * Multiplication and division use 64-bit intermediates to avoid overflow.
 * On the SH-4A, 64-bit multiply is slow (software), but still faster than
 * soft-float — and we avoid it in hot loops by precomputing reciprocals
 * where possible.
 *
 * For trig, see math_lut.h — we have BRAD-based (binary radian) sin/cos
 * tables that pair perfectly with fix16_t angles.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t fix16_t;

/* Conversions. */
#define FIX16_ONE       (65536)             /* 1.0 in 16.16 */
#define FIX16_HALF      (32768)             /* 0.5 */
#define FIX16_TWO       (131072)            /* 2.0 */
#define FIX16_PI        (205887)            /* 3.14159... */
#define FIX16_2PI       (411775)            /* 6.28318... */

/* int -> fix16 */
static inline fix16_t fix16_from_int(int v) { return (fix16_t)(v << 16); }

/* fix16 -> int (truncates) */
static inline int fix16_to_int(fix16_t v) { return (int)(v >> 16); }

/* fix16 -> int rounded */
static inline int fix16_to_int_round(fix16_t v) {
    return (int)((v + FIX16_HALF) >> 16);
}

/* fix16_mul: a * b. Uses 64-bit intermediate to avoid overflow. */
static inline fix16_t fix16_mul(fix16_t a, fix16_t b) {
    return (fix16_t)(((int64_t)a * b) >> 16);
}

/* fix16_div: a / b. Uses 64-bit intermediate. */
static inline fix16_t fix16_div(fix16_t a, fix16_t b) {
    if (b == 0) return 0;
    return (fix16_t)(((int64_t)a << 16) / b);
}

/* fix16_recip: 1 / a. Convenience for fix16_div(FIX16_ONE, a). */
static inline fix16_t fix16_recip(fix16_t a) {
    if (a == 0) return 0;
    return (fix16_t)(((int64_t)FIX16_ONE << 16) / a);
}

/* fix16_abs: |a| (careful with INT32_MIN, but that won't happen here). */
static inline fix16_t fix16_abs(fix16_t a) { return a < 0 ? -a : a; }

/* fix16_floor: floor(a) as fix16. */
static inline fix16_t fix16_floor(fix16_t a) { return a & 0xFFFF0000; }

/* fix16_frac: fractional part as fix16 (always in [0, 1)). */
static inline fix16_t fix16_frac(fix16_t a) { return a & 0xFFFF; }

/* Linear interpolation: a + (b - a) * t, where t is fix16 in [0, 1]. */
static inline fix16_t fix16_lerp(fix16_t a, fix16_t b, fix16_t t) {
    return a + fix16_mul(b - a, t);
}

/* fix16_sqrt: integer Babylonian method. Returns floor(sqrt(a)). */
static inline fix16_t fix16_sqrt(fix16_t a) {
    if (a <= 0) return 0;
    /* sqrt(a) in 16.16 = sqrt(a * 2^32) / 2^16 = sqrt(a << 16). */
    uint64_t n = (uint64_t)a << 16;
    uint64_t x = n, y = (x + 1) / 2;
    while (y < x) { x = y; y = (x + n / x) / 2; }
    return (fix16_t)x;
}

#ifdef __cplusplus
}
#endif
