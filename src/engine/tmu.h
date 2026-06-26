/* src/engine/tmu.h */
/*
 * cpcraft-port — engine/tmu.h
 *
 * SH-4A Timer Unit (TMU) — used by the engine for cycle-accurate frame timing.
 *
 * Why we keep this:
 *   The benchmark used TMU channel 1 to measure render/refresh ticks. We do
 *   the same in the demo so the user can verify on hardware that our engine
 *   actually hits the ~140k-tick budget that the benchmark proved achievable.
 *
 * Channel 0 is left for the OS, channel 1 is what we use, channel 2 is free.
 *
 * Ported from Render-Display-Benchmark/src/tmu.h (QBos07) — same register
 * layout, same prescaler constants.
 *
 * SIMULATOR NOTE: When building for the simulator (i.e. not __sh__), the
 * TMU register macros point at real host-side storage so the engine's
 * writes don't segfault. The simulator also overrides TMU_TCNT_1 to a
 * function that returns a high-resolution host timer tick (so the engine's
 * tick-delta measurements still produce sensible numbers). */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TMU counts down at PHI/TPSC. With PHI = ~58 MHz and TPSC=PHI_DIV_4 we get
 * roughly 14.5 MHz -> ~14.5 ticks per microsecond. The benchmark called this
 * 8 ticks/usec; the exact value doesn't matter for relative frame timings
 * (we only compare deltas). */
#define TMU_TICKS_PER_USEC  8
#define TMU_TICKS_PER_SEC   (TMU_TICKS_PER_USEC * 1000 * 1000)

typedef enum
{
  PHI_DIV_4     = 0x0,
  PHI_DIV_16    = 0x1,
  PHI_DIV_64    = 0x2,
  PHI_DIV_256   = 0x3,
  PHI_DIV_1024  = 0x4
} tmu_tcr_tpsc;

/* NOTE: typedef'd so the macros below don't need the `union`/`enum` keyword.
 * Same reason as power.h: original benchmark was C++. */
typedef union
{
  struct
  {
    uint8_t _reserved0  : 5;
    uint8_t STR2        : 1;
    uint8_t STR1        : 1;
    uint8_t STR0        : 1;
  } s;
  uint8_t raw;
} tmu_tstr;

typedef union
{
  struct
  {
    uint16_t _reserved0 : 7;
    uint16_t UNF        : 1; /* Underflow Flag     */
    uint16_t _reserved1 : 2;
    uint16_t UNIE       : 1; /* Underflow Interrupt Enable */
    uint16_t _reserved2 : 2;
    tmu_tcr_tpsc TPSC   : 3; /* Timer Prescaler */
  } s;
  uint16_t raw;
} tmu_tcr;

#ifdef __sh__
/* Real hardware: TMU registers are at the fixed MMIO addresses. */

/* General register — uses the typedef'd union. */
#define TMU_TSTR    ((volatile tmu_tstr *) 0xA4490004)

/* Channel 0 */
#define TMU_TCOR_0  ((volatile uint32_t *) 0xA4490008)
#define TMU_TCNT_0  ((volatile uint32_t *) 0xA449000C)
#define TMU_TCR_0   ((volatile tmu_tcr *)  0xA4490010)

/* Channel 1 — used by the engine for frame timing */
#define TMU_TCOR_1  ((volatile uint32_t *) 0xA4490014)
#define TMU_TCNT_1  ((volatile uint32_t *) 0xA4490018)
#define TMU_TCR_1   ((volatile tmu_tcr *)  0xA449001C)

/* Channel 2 */
#define TMU_TCOR_2  ((volatile uint32_t *) 0xA4490020)
#define TMU_TCNT_2  ((volatile uint32_t *) 0xA4490024)
#define TMU_TCR_2   ((volatile tmu_tcr *)  0xA4490028)

#else
/* Simulator: TMU registers point at real host-side storage so the engine's
 * writes don't segfault. The simulator runtime (simulator.cpp) defines
 * these. sim_tmu_tcnt_1_storage is updated by the simulator's main loop
 * on each frame; the engine reads it via *TMU_TCNT_1 to compute frame
 * deltas. Numbers won't be cycle-accurate vs the real 14.5 MHz TMU clock,
 * but they'll be in the same ballpark — enough to spot regressions. */
extern tmu_tstr sim_tmu_tstr;
extern tmu_tcr  sim_tmu_tcr_0, sim_tmu_tcr_1, sim_tmu_tcr_2;
extern uint32_t sim_tmu_tcor_0, sim_tmu_tcor_1, sim_tmu_tcor_2;
extern uint32_t sim_tmu_tcnt_0, sim_tmu_tcnt_1_storage, sim_tmu_tcnt_2;

#define TMU_TSTR    ((volatile tmu_tstr *) &sim_tmu_tstr)
#define TMU_TCOR_0  ((volatile uint32_t *) &sim_tmu_tcor_0)
#define TMU_TCNT_0  ((volatile uint32_t *) &sim_tmu_tcnt_0)
#define TMU_TCR_0   ((volatile tmu_tcr *)  &sim_tmu_tcr_0)
#define TMU_TCOR_1  ((volatile uint32_t *) &sim_tmu_tcor_1)
#define TMU_TCNT_1  ((volatile uint32_t *) &sim_tmu_tcnt_1_storage)
#define TMU_TCR_1   ((volatile tmu_tcr *)  &sim_tmu_tcr_1)
#define TMU_TCOR_2  ((volatile uint32_t *) &sim_tmu_tcor_2)
#define TMU_TCNT_2  ((volatile uint32_t *) &sim_tmu_tcnt_2)
#define TMU_TCR_2   ((volatile tmu_tcr *)  &sim_tmu_tcr_2)
#endif

#ifdef __cplusplus
}
#endif
