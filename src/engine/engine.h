/*
 * cpcraft-port — engine/engine.h
 *
 * Umbrella header — pull this in to get the whole engine API.
 *
 * Layout of the engine module:
 *
 *   engine/
 *     builtins.h      - likely/unlikely, ENGINE_INLINE
 *     power.h         - MSTPCR0 layout (for un-gating clocks)
 *     tmu.h           - TMU channel 1 (frame timing)
 *     framebuffer.{c,h} - the star of the show: 160x264 virtual FB,
 *                         YRAM-backed line pools, 2x-upscale blit
 *     input.{c,h}     - polled key state with edge detection
 *     math_lut.{c,h}  - 8-bit sin LUT + 16.16 fixed-point sin/cos table
 *     overlay.{c,h}   - tiny 4x6 font + printf for on-screen labels
 *     texture.{c,h}   - procedural 16x16 textures + textured rect draw
 *
 * Why this layout:
 *   Each file is a self-contained concept. CPCraft's original engine.c was
 *   ~30k lines of mixed concerns. By keeping the base engine strictly
 *   modular, we can port cpcraft features one at a time without dragging
 *   in unrelated code.
 */
#pragma once

/* Pull in all engine headers. Includes go OUTSIDE the extern "C" block
 * so that the simulator's <sdk/...> headers can use C++ features (e.g.
 * the getKey(Keys1*, Keys2*) overload in <sdk/calc/calc.h>) without
 * being forced into C linkage. */
#include "builtins.h"
#include "power.h"
#include "tmu.h"
#include "framebuffer.h"
#include "input.h"
#include "math_lut.h"
#include "overlay.h"
#include "texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One-stop initialization. Call once at startup, before any other engine
 * function. Initializes the framebuffer, input state, and procedural
 * textures. */
void engine_init(void);

#ifdef __cplusplus
}
#endif