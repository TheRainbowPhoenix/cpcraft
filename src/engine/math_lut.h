/* src/engine/math_lut.h */
/*
 * cpcraft-port — engine/math_lut.h
 *
 * Pre-computed lookup tables — PLAIN INT, no fixed-point.
 *
 * Sin/cos tables use BRAD angles (0..65535 = 0..2π) and return
 * int32_t scaled by 10000 (so 1.0 = 10000, -1.0 = -10000).
 *
 * World positions use 100 units per block (so 5.12 blocks = 512).
 * This means all math is plain int32_t — no shifts, no fix16_mul.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 8-bit unsigned sin, period 256, range [0, 255], centered at 128. */
extern const uint8_t sin_lut8[256];

static inline uint8_t fast_sin8(uint8_t phase) { return sin_lut8[phase]; }
static inline uint8_t fast_cos8(uint8_t phase) { return sin_lut8[(uint8_t)(phase + 64)]; }

uint8_t isin(uint16_t phase16);

/* --- BRAD sin/cos: 0..65535 = 0..2pi, scale 10000 (1.0 = 10000) --- */
extern const int32_t sin_table_brads[256];

/* sin of a BRAD angle. Returns int32_t in [-10000, 10000]. */
static inline int32_t sin_brads(uint16_t angle) {
    return sin_table_brads[(uint8_t)(angle >> 8)];
}

/* cos of a BRAD angle = sin(angle + π/2) = sin(angle + 16384). */
static inline int32_t cos_brads(uint16_t angle) {
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

/* --- Old fix16 compat (deprecated, do not use in new code) --- */
extern const int32_t sintable_deg10[450];
static inline int32_t fsin_deg10(int a) {
    while (a < 0) a += 3600;
    while (a >= 3600) a -= 3600;
    return sintable_deg10[a];
}
static inline int32_t fcos_deg10(int a) { return fsin_deg10(a + 900); }

#ifdef __cplusplus
}
#endif
