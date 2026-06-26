/* src/engine/fix16.h */
/*
 * cpcraft-port — engine/fix16.h
 *
 * 16.16 fixed-point arithmetic (32-bit only, NO int64_t).
 *
 * The ClassPad's SH-4A is a 32-bit processor. Using int64_t causes
 * address errors because the compiler generates 64-bit shift/multiply
 * code that requires 8-byte alignment, which isn't guaranteed on the
 * stack.
 *
 * The fix16_mul and fix16_div implementations below are ported from
 * libfixmath's FIXMATH_NO_64BIT and FIXMATH_NO_HARD_DIVISION versions
 * (https://github.com/PetteriAimonen/libfixmath). They use only 32-bit
 * operations (16*16->32 multiply, restoring division).
 *
 * Type: fix16_t = int32_t, with 16 integer bits and 16 fractional bits.
 *   Range: [-32768.0, 32767.99998]
 *   Resolution: 1/65536 ≈ 0.0000153
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t fix16_t;

/* Conversions. */
#define FIX16_ONE       (65536)
#define FIX16_HALF      (32768)
#define FIX16_TWO       (131072)
#define FIX16_PI        (205887)
#define FIX16_2PI       (411775)

static inline fix16_t fix16_from_int(int v) { return (fix16_t)(v << 16); }
static inline int fix16_to_int(fix16_t v) { return (int)(v >> 16); }
static inline int fix16_to_int_round(fix16_t v) {
    return (int)((v + FIX16_HALF) >> 16);
}

static inline fix16_t fix16_abs(fix16_t a) { return a < 0 ? -a : a; }
static inline fix16_t fix16_floor(fix16_t a) { return a & 0xFFFF0000; }
static inline fix16_t fix16_frac(fix16_t a) { return a & 0xFFFF; }

/* fix16_mul: 32-bit-only implementation.
 * Splits each argument into 16-bit halves and uses 16*16->32 multiplies.
 * No int64_t — safe on SH-4A.
 *
 * Ported from libfixmath's FIXMATH_NO_64BIT version. */
static inline fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1)
{
    /* Each argument is divided to 16-bit parts.
     *   AB * CD = AC*65536 + (AD+CB)*256 + BD/65536 (approx)
     * We compute: product_hi = AC + (AD+CB) >> 16, then combine with BD. */
    int32_t A = (inArg0 >> 16), C = (inArg1 >> 16);
    uint32_t B = (inArg0 & 0xFFFF), D = (inArg1 & 0xFFFF);

    int32_t AC = A * C;
    int32_t AD_CB = A * D + C * B;
    uint32_t BD = B * D;

    int32_t product_hi = AC + (AD_CB >> 16);

    /* Handle carry from lower 32 bits to upper part. */
    uint32_t ad_cb_temp = AD_CB << 16;
    uint32_t product_lo = BD + ad_cb_temp;
    if (product_lo < BD)
        product_hi++;

    /* Round: subtract 0x8000 (0.5) then shift. */
    uint32_t product_lo_tmp = product_lo;
    product_lo -= 0x8000;
    product_lo -= (uint32_t)product_hi >> 31;
    if (product_lo > product_lo_tmp)
        product_hi--;

    fix16_t result = (product_hi << 16) | (product_lo >> 16);
    result += 1;
    return result;
}

/* fix16_div: 32-bit-only restoring division.
 * Ported from libfixmath's FIXMATH_NO_HARD_DIVISION version.
 * No int64_t — safe on SH-4A. */
static inline fix16_t fix16_div(fix16_t a, fix16_t b)
{
    if (b == 0) return 0;  /* caller should check */

    uint32_t remainder = fix16_abs(a);
    uint32_t divider = fix16_abs(b);
    uint32_t quotient = 0;
    uint32_t bit = 0x10000;

    /* The algorithm requires D >= R. */
    while (divider < remainder) {
        divider <<= 1;
        bit <<= 1;
}

    if (divider & 0x80000000) {
        if (remainder >= divider) {
            quotient |= bit;
            remainder -= divider;
        }
        divider >>= 1;
        bit >>= 1;
}

    /* Main division loop. */
    while (bit && remainder) {
        if (remainder >= divider) {
            quotient |= bit;
            remainder -= divider;
        }
        remainder <<= 1;
        bit >>= 1;
    }

    if (remainder >= divider)
        quotient++;

    fix16_t result = quotient;

    if ((a ^ b) & 0x80000000)
        result = -result;

    return result;
}

static inline fix16_t fix16_recip(fix16_t a) {
    if (a == 0) return 0;
    return fix16_div(FIX16_ONE, a);
}

/* Linear interpolation: a + (b - a) * t. */
static inline fix16_t fix16_lerp(fix16_t a, fix16_t b, fix16_t t) {
    return a + fix16_mul(b - a, t);
}

/* fix16_sqrt: integer Babylonian method (32-bit only). */
static inline fix16_t fix16_sqrt(fix16_t a) {
    if (a <= 0) return 0;
    /* sqrt(a) in 16.16 ≈ sqrt(a) * 256.
     * Use 32-bit math: start with a guess and iterate. */
    uint32_t n = (uint32_t)a;
    uint32_t x = n, y = (x + 1) / 2;
    while (y < x) { x = y; y = (x + n / x) / 2; }
    /* x is now sqrt(a) in 0.16 format. Shift to 16.16. */
    return (fix16_t)(x << 8);
}

#ifdef __cplusplus
}
#endif
