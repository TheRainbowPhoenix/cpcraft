/* src/engine/fix16.h */
/*
 * cpcraft-port — engine/fix16.h
 *
 * 16.16 fixed-point type — PLAIN INT, no function calls.
 *
 * fix16_t is just int32_t. The "16.16" means the lower 16 bits are
 * the fractional part. 1.0 = 65536, 0.5 = 32768, etc.
 *
 * We do NOT use any fix16_mul/fix16_div functions. Instead:
 *   - Multiplication: (a * b) >> 16  (only when both fit in 16 bits)
 *   - Division:       (a << 16) / b  (only when result fits)
 *   - Or just use plain int and avoid fixed-point entirely where possible.
 *
 * The ClassPad's SH-4A is 32-bit. Using int64_t causes address errors.
 * Using float causes crashes (no FPU). So we use ONLY int32_t and
 * uint32_t, with careful attention to overflow.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* fix16_t is just int32_t. We keep the typedef for readability. */
typedef int32_t fix16_t;

/* Constants. */
#define FIX16_ONE       (65536)
#define FIX16_HALF      (32768)
#define FIX16_TWO       (131072)
#define FIX16_PI        (205887)
#define FIX16_2PI       (411775)

/* Conversions (inline, no function call overhead). */
#define fix16_from_int(v)      ((int32_t)((v) << 16))
#define fix16_to_int(v)        ((int)((v) >> 16))
#define fix16_to_int_round(v)  ((int)(((v) + FIX16_HALF) >> 16))

/* Absolute value. */
static inline int32_t fix16_abs(int32_t a) { return a < 0 ? -a : a; }

/* Floor and fractional part. */
#define fix16_floor(v)  ((int32_t)((v) & 0xFFFF0000))
#define fix16_frac(v)   ((int32_t)((v) & 0xFFFF))

/*
 * fix16_mul: 32-bit-only multiply.
 *
 * Splits each argument into 16-bit halves and uses 16*16->32 multiplies.
 * This is the SAME algorithm as libfixmath's FIXMATH_NO_64BIT version,
 * but inlined as a static function. No int64_t.
 *
 * Only use this when you MUST multiply two fix16_t values. In hot loops,
 * prefer to precompute reciprocals or use plain int.
 */
static inline int32_t fix16_mul(int32_t inArg0, int32_t inArg1)
{
    int32_t A = (inArg0 >> 16), C = (inArg1 >> 16);
    uint32_t B = (inArg0 & 0xFFFF), D = (inArg1 & 0xFFFF);

    int32_t AC = A * C;
    int32_t AD_CB = A * D + C * B;
    uint32_t BD = B * D;

    int32_t product_hi = AC + (AD_CB >> 16);

    uint32_t ad_cb_temp = AD_CB << 16;
    uint32_t product_lo = BD + ad_cb_temp;
    if (product_lo < BD)
        product_hi++;

    /* Overflow check. */
    if (product_hi >> 15 != product_hi >> 31) {
        return (inArg0 < 0) ^ (inArg1 < 0) ? (int32_t)0x80000000 : (int32_t)0x7FFFFFFF;
    }

    uint32_t product_lo_tmp = product_lo;
    product_lo -= 0x8000;
    product_lo -= (uint32_t)product_hi >> 31;
    if (product_lo > product_lo_tmp)
        product_hi--;

    int32_t result = (product_hi << 16) | (product_lo >> 16);
    result += 1;
    return result;
}

/*
 * fix16_div: 32-bit-only restoring division.
 * Same algorithm as libfixmath's FIXMATH_NO_HARD_DIVISION version.
 * No int64_t.
 */
static inline int32_t fix16_div(int32_t a, int32_t b)
{
    if (b == 0) return 0;

    uint32_t remainder = fix16_abs(a);
    uint32_t divider = fix16_abs(b);

    if (divider < 16 && remainder > 0x10000000)
        return (a < 0) ^ (b < 0) ? (int32_t)0x80000000 : (int32_t)0x7FFFFFFF;

    uint32_t quotient = 0;
    uint32_t bit = 0x10000;

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

    int32_t result = (int32_t)quotient;

    if ((a ^ b) & 0x80000000)
        result = -result;

    return result;
}

#define fix16_recip(a)  fix16_div(FIX16_ONE, (a))

/* Linear interpolation: a + (b - a) * t. */
static inline int32_t fix16_lerp(int32_t a, int32_t b, int32_t t) {
    return a + fix16_mul(b - a, t);
}

#ifdef __cplusplus
}
#endif
