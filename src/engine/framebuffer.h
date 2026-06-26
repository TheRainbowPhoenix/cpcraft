/* src/engine/framebuffer.h */
/*
 * cpcraft-port — engine/framebuffer.h
 *
 * Fast framebuffer + blit routines for the ClassPad LCD.
 *
 * ============================================================================
 *  Why this design (read this before changing anything)
 * ============================================================================
 *
 *  QBos07's Render-Display-Benchmark measured every reasonable way to push
 *  pixels to the ClassPad LCD. The headline result:
 *
 *      Pixel+Line + YRAM buffer + 2x upscale  =  ~140 000 ticks / full frame
 *      Line       + YRAM buffer + 2x upscale  =  ~199 000 ticks / full frame
 *      Full       + YRAM buffer + 2x upscale  =  ~197 000 ticks / full frame
 *      Full       + plain CPU                 =  ~470 000 ticks / full frame
 *      Full       + DMAC                      =  ~724 000 ticks / full frame   <- SLOWER
 *
 *  Three non-obvious lessons:
 *
 *   1.  DMA is SLOWER than CPU on this LCD. The LCD data port sits on a slow
 *       bus; the CPU can stream writes through the store queue and pipeline
 *       them, while the DMAC stalls between every transfer. So we DON'T use
 *       DMAC for blitting, period.
 *
 *   2.  A per-line YRAM buffer wins. YRAM (`.oc_mem.y` section, ~8 KiB on the
 *       ClassPad) is on a fast bus that doesn't compete with VRAM reads.
 *       Rendering one scanline into YRAM, then streaming it to the LCD, lets
 *       the CPU pipeline the next line's render with the previous line's
 *       refresh. With two buffers we double-buffer the render/refresh so
 *       there's no stall between lines.
 *
 *   3.  2x upscaling is essentially free AND cuts render cost 4x. We render
 *       into a 160x264 framebuffer; the blit doubles each pixel both
 *       horizontally and vertically when streaming to the LCD. The doubling
 *       loop is tiny (two `*lcd_data_port = pixel` writes per source pixel)
 *       and is dominated by the LCD's own write latency, so the extra write
 *       is hidden. Meanwhile the renderer only computes 1/4 as many pixels.
 *
 *  Net result: a 160x264 RGB565 framebuffer with double-buffered YRAM line
 *  pools and a 2x-upscale blit gives us ~140k ticks/frame, leaving plenty of
 *  headroom for actual game logic on a 14.5 MHz tick clock.
 *
 * ============================================================================
 *  API summary
 * ============================================================================
 *
 *    fb_init()              - call once at startup (allocates YRAM pools)
 *    fb_clear(color)        - fill the virtual framebuffer with a color
 *    fb_pixel(x, y, color)  - set a pixel in the 160x264 virtual framebuffer
 *    fb_get_pixel(x, y)     - read a pixel (e.g. for texture sampling)
 *    fb_hline(x1, x2, y, c) - fast horizontal line in the framebuffer
 *    fb_vline(x, y1, y2, c) - fast vertical line in the framebuffer
 *    fb_rect(x, y, w, h, c) - filled rectangle
 *    fb_present()           - blit framebuffer to the LCD with 2x upscaling
 *
 *  Coordinates: x in [0, FB_W), y in [0, FB_H). No clipping is done in
 *  fb_pixel for speed — use the rect/hline/vline helpers if you need safety.
 *
 *  The framebuffer itself lives in main VRAM (0x8c000000), not YRAM, because
 *  160*264*2 = 84 480 bytes which is way larger than YRAM. Only the per-line
 *  transfer pools live in YRAM. The renderer writes to VRAM; the presenter
 *  copies one VRAM line into a YRAM pool, then streams that pool to the LCD
 *  with 2x doubling. This matches the benchmark's MODE_PIXEL+LINE+USE_UP+USE_Y
 *  configuration exactly.
 */
#pragma once

#include <stdint.h>
#include "builtins.h"

/* Do not include <sdk/os/lcd.h> or <sdk/calc/calc.h> here. They have C++
 * overloads (getKey(Keys1*, Keys2*) etc.) and pull in OS-specific typedefs
 * that can leak into C translation units. The .c file includes them
 * directly when it needs the LCD primitives. */

#ifdef __cplusplus
extern "C" {
#endif

/* SIM_LCD_WRITE(pixel): stream one pixel to the LCD.
 *
 * On hardware this is just `*lcd_data_port = (pixel)` — a volatile store
 * to memory-mapped I/O that the LCD controller picks up.
 *
 * The simulator's <sdk/os/lcd.h> overrides this macro to call
 * sim_lcd_write(pixel) instead, which lets the simulator intercept the
 * write and update its LCD framebuffer.
 *
 * If <sdk/os/lcd.h> didn't define SIM_LCD_WRITE (i.e. we're building
 * against the real SDK), we define it here as the plain store.
 *
 * NOTE: framebuffer.c includes <sdk/os/lcd.h> before this header, so by
 * the time we get here the macro is already defined (or not, in which
 * case we define it to the hardware form). */
#ifndef SIM_LCD_WRITE
#define SIM_LCD_WRITE(pixel) (*lcd_data_port = (pixel))
#endif

/* Virtual framebuffer size. Half the LCD in each axis -> 2x upscale.
 *
 * We hardcode 160x264 (matching the ClassPad's 320x528 LCD) instead of
 * deriving from the SDK's `width` / `height` constants, because those
 * are `static const` (not `#define`) and therefore not usable as array
 * bounds in C file-scope declarations.
 *
 * LCD_W / LCD_H are the physical LCD dimensions. We use these instead of
 * the SDK's `width` / `height` because we don't include <sdk/calc/calc.h>
 * from this header (it has C++ overloads that break C translation units).
 */
#define LCD_W   320
#define LCD_H   528
#define FB_W    (LCD_W / 2)   /* 160 */
#define FB_H    (LCD_H / 2)   /* 264 */

/* YRAM-backed line pools were used in the earlier per-line-strip version
 * of fb_present(). The current version writes straight from VRAM to the
 * LCD with a single full-screen window, so no intermediate buffer is
 * needed. The .oc_mem.y.fb section attribute is kept here as a comment
 * for reference — if a future optimization needs YRAM again, restore
 * the attribute on a static buffer. */

/* The virtual framebuffer itself, in main VRAM.
 *
 * We use the SDK's vram pointer (0x8c000000) as our backing store but only
 * the top-left FB_W*FB_H pixels of it; the presenter overwrites the whole
 * screen anyway. Reading / writing VRAM is fast enough on the ClassPad; the
 * slow part is talking to the LCD, which is what the YRAM pools fix. */
extern uint16_t *fb_vram;

/* Initialize the framebuffer (called once at startup). */
void fb_init(void);

/* Fill the virtual framebuffer with a single color. */
void fb_clear(uint16_t color);

/* Set / get a pixel in the 160x264 framebuffer. Inline for speed. */
ENGINE_INLINE void fb_pixel(int x, int y, uint16_t color)
{
    /* No bounds check in the hot path. Callers stay in-range. */
    fb_vram[y * FB_W + x] = color;
}

ENGINE_INLINE uint16_t fb_get_pixel(int x, int y)
{
    return fb_vram[y * FB_W + x];
}

/* Fast horizontal line in the framebuffer. */
void fb_hline(int x1, int x2, int y, uint16_t color);

/* Fast vertical line in the framebuffer. */
void fb_vline(int x, int y1, int y2, uint16_t color);

/* Filled rectangle in the framebuffer. */
void fb_rect(int x, int y, int w, int h, uint16_t color);

/* Present the virtual framebuffer to the LCD with 2x upscaling.
 *
 * Walks the framebuffer one source line at a time:
 *   1. Copy the source line into a YRAM pool (cache-writeback follows).
 *   2. Stream the pool to the LCD twice (for the 2x vertical doubling),
 *      with each pixel written twice (for the 2x horizontal doubling).
 * The two YRAM pools are double-buffered so step 1 of line N+1 can overlap
 * with step 2 of line N — that's where the speedup comes from.
 *
 * Uses TMU channel 1 to record refresh ticks in `fb_last_refresh_ticks`
 * (so the demo can show them on screen). */
void fb_present(void);

/* Last frame's refresh tick count (set by fb_present). */
extern uint32_t fb_last_refresh_ticks;

#ifdef __cplusplus
}
#endif
