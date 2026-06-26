/*
 * cpcraft-port — engine/power.h
 *
 * SH-4A Module Stop Control Register (MSTPCR0) layout.
 *
 * The ClassPad's OS leaves most peripheral clocks gated off; before we touch
 * the TMU (for frame timing) or the DMAC (kept here for reference even though
 * we don't use it for blitting — the benchmark proved CPU writes win), we have
 * to un-gate them by clearing the corresponding bit in MSTPCR0.
 *
 * Ported from Render-Display-Benchmark/src/power.h (QBos07) — same bit layout.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* NOTE: typedef'd so the macro below doesn't need the `union` keyword.
 * The original benchmark was C++ where union tags work as type names
 * directly; in C we need either `union power_mstpcr0` or a typedef. */
typedef union power_mstpcr0
{
  struct
  {
    uint32_t TLB        : 1;
    uint32_t IC         : 1;
    uint32_t OC         : 1;
    uint32_t _reserved0 : 1;
    uint32_t IL         : 1;
    uint32_t _reserved1 : 2;
    uint32_t FPU        : 1;
    uint32_t _reserved2 : 1;
    uint32_t INTC       : 1;
    uint32_t DMAC       : 1;
    uint32_t _reserved3 : 1;
    uint32_t HUDI       : 1;
    uint32_t DBG        : 1;
    uint32_t UBC        : 1;
    uint32_t SUBC       : 1;
    uint32_t TMU        : 1;
    uint32_t CMT        : 1;
    uint32_t RWDT       : 1;
    uint32_t _reserved4 : 3;
    uint32_t SCIF4      : 1;
    uint32_t SCIF5      : 1;
    uint32_t SCIF0      : 1;
    uint32_t SCIF1      : 1;
    uint32_t SCIF2      : 1;
    uint32_t SCIF3      : 1;
    uint32_t _reserved5 : 1;
    uint32_t SIOF       : 1;
    uint32_t _reserved6 : 2;
  } s;

  uint32_t raw;
} power_mstpcr0;

#define POWER_MSTPCR0 ((volatile power_mstpcr0 *)0xA4150030)

#ifdef __cplusplus
}
#endif
