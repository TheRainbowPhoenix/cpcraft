/* src/engine/engine.h */
/*
 * cpcraft-port — engine/engine.h
 *
 * Umbrella header — pull this in to get the whole engine API.
 *
 * Layout of the engine module:
 *
 *   engine/
 *     builtins.h      - likely/unlikely, ENGINE_INLINE
 *     math_types.h      - Vec2i/Vec3i/Vec4b/etc.
 *     power.h         - MSTPCR0 layout (for un-gating clocks)
 *     tmu.h           - TMU channel 1 (frame timing)
 *     math_lut.{c,h}  - 8-bit sin LUT + 16.16 fixed-point sin/cos table
 *     framebuffer.{c,h} - 160x264 virtual FB + direct-VRAM-to-LCD blit
 *     input.{c,h}       - polled key state with edge detection
 *     overlay.{c,h}   - tiny 4x6 font + printf for on-screen labels
 *     texture.{c,h}   - procedural 16x16 textures + textured rect draw
 *     world.{c,h}       - voxel world storage + terrain generation
 *     player.{c,h}      - player position/rotation/velocity + collision
 *     camera.{c,h}      - 3D-to-2D projection
 *     rasterizer.{c,h}  - z-buffered textured quad rasterizer
 *     block_render.{c,h}- walk world, draw exposed cube faces
 *     ui.{c,h}          - crosshair, hotbar, debug overlay
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
#include "math_types.h"
#include "fix16.h"
#include "power.h"
#include "tmu.h"
#include "framebuffer.h"
#include "input.h"
#include "math_lut.h"
#include "overlay.h"
#include "texture.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "rasterizer.h"
#include "raycast.h"
#include "block_render.h"
#include "game_data.h"
#include "ui.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One-stop initialization. Call once at startup, before any other engine
 * function. Initializes the framebuffer, input state, procedural textures,
 * world, player, and block texture table. */
void engine_init(void);

#ifdef __cplusplus
}
#endif
