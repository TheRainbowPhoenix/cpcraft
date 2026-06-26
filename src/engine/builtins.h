/* src/engine/builtins.h */
/*
 * cpcraft-port — engine/builtins.h
 *
 * Tiny compiler-arc helper macros shared across the engine.
 *
 * Why a separate header?
 *   - Every hot loop in the engine uses likely()/unlikely() for branch hints.
 *   - `always_inline` keeps the blit / line-draw routines from becoming real
 *     function calls (which would clobber r4..r7 register allocation on SH-4A
 *     and cost us the cycle budget the benchmark proved we need).
 *
 * This file is intentionally minimal — only what the engine needs.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Force inlining even at -Os. The blit routines are called per-line so any
 * call overhead (pr save/restore + stack frame) shows up at the frame level. */
#define ENGINE_INLINE static inline __attribute__((always_inline))

#ifdef __cplusplus
}
#endif
