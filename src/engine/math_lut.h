/* src/engine/math_lut.h */
/*
 * cpcraft-port — engine/math_lut.h
 *
 * Pre-computed sin/cos lookup tables, 256 entries each.
 *
 * Why 256?
 *   - The benchmark used a 256-entry sin LUT indexed by an 8-bit phase
 *     accumulator, which lets us advance the phase with a single `add`
 *     instruction and index with a single `and #0xFF`. This is the cheapest
 *     way to do per-pixel sinusoids on the SH-4A.
 *   - 256 entries is enough resolution for plasma / wave effects; if you
 *     need sub-degree accuracy, linearly interpolate between two entries
 *     (we provide isin() for that).
 *
 * Values: 8-bit unsigned (0..255), centered at 128, amplitude 127. This
 * matches the benchmark's plasma generator exactly so we can reuse its
 * rendering trick of summing three LUT lookups and shifting down.
 *
 * For angles in 16.16 fixed point, see fsin() / fcos() below — those are
 * the right thing for raycaster-style code (taken from CP-Raycaster-Demo).
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 8-bit unsigned sin, period 256, range [0, 255], centered at 128. */
extern const uint8_t sin_lut8[256];

/* Quick 8-bit sin. Phase is in [0, 256). */
static inline uint8_t fast_sin8(uint8_t phase) { return sin_lut8[phase]; }

/* Quick 8-bit cos. Phase is in [0, 256). cos(x) = sin(x + 64). */
static inline uint8_t fast_cos8(uint8_t phase) { return sin_lut8[(uint8_t)(phase + 64)]; }

/* Interpolated 8-bit sin for slightly smoother output. Phase is 16-bit
 * (0..65535 maps to 0..2pi). Returns [0, 255]. */
uint8_t isin(uint16_t phase16);

/* --- 16.16 fixed-point sin/cos (degrees, period 360) --------------------- */
/* Lifted from CP-Raycaster-Demo/src/math_tables.h — useful for raycaster-
 * style math where you need real angle arithmetic, not just LUT lookups.
 *
 * Argument: angle in 1/10ths of a degree (so 900 = 90deg, 3600 = 360deg).
 * Returns:  -65536..65535, where 65536 == 1.0.
 *
 * The table has 450 entries (covers 0..450 degrees with wraparound). */
extern const int32_t sintable_deg10[450];

static inline int32_t fsin_deg10(int angle_deg10)
{
    while (angle_deg10 < 0)    angle_deg10 += 3600;
    while (angle_deg10 >= 3600) angle_deg10 -= 3600;
    return sintable_deg10[angle_deg10];
}

static inline int32_t fcos_deg10(int angle_deg10)
{
    return fsin_deg10(angle_deg10 + 900);
}

#ifdef __cplusplus
}
#endif
