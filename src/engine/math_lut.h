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
#include "fix16.h"

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

/* --- BRAD (binary radian) sin/cos: 0..65535 = 0..2pi --------------------- */
/*
 * The fastest possible angle representation for 16.16 fixed-point code.
 *
 * A BRAD angle is a uint16_t where:
 *   0     = 0 radians
 *   16384 = π/2 (90°)
 *   32768 = π   (180°)
 *   49152 = 3π/2 (270°)
 *   65535 ≈ 2π  (360°)
 *
 * Advantages:
 *   - Angle wraparound is a single `& 0xFFFF` (or just storing in a uint16_t).
 *   - No conditional branches for quadrant wrapping (unlike degree-based).
 *   - Table has 256 entries (index = angle >> 8), each a fix16_t.
 *   - cos(a) = sin(a + 16384) — just add and mask.
 *
 * Returns: fix16_t in [-65536, 65536] (= [-1.0, 1.0] in 16.16).
 *
 * Generation:
 *   sin_table[i] = (fix16_t)(sin(i / 256.0 * 2π) * 65536.0)
 * for i in [0, 256).
 */
extern const fix16_t sin_table_brads[256];

/* sin of a BRAD angle. The angle is masked to [0, 65535] first. */
static inline fix16_t fix16_sin_brads(uint16_t angle) {
    return sin_table_brads[(uint8_t)(angle >> 8)];
}

/* cos of a BRAD angle = sin(angle + π/2) = sin(angle + 16384). */
static inline fix16_t fix16_cos_brads(uint16_t angle) {
    return sin_table_brads[(uint8_t)((angle + 16384) >> 8)];
}

/* Convert degrees to BRAD: brad = degrees * 65536 / 360. */
static inline uint16_t deg_to_brads(int degrees) {
    while (degrees < 0)    degrees += 360;
    while (degrees >= 360) degrees -= 360;
    return (uint16_t)(degrees * 65536 / 360);
}

/* Convert BRAD to degrees (for debug printing). */
static inline int brads_to_deg(uint16_t angle) {
    return (int)(angle * 360 / 65536);
}

#ifdef __cplusplus
}
#endif
