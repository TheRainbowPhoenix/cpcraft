/*
 * cpcraft-port — engine/overlay.h
 *
 * Minimal text overlay drawn on top of the virtual framebuffer.
 *
 * The SDK's Debug_Printf writes to VRAM using the system font (8x12 glyphs)
 * and then the caller has to LCD_Refresh(). That's fine for debugging but:
 *   1. It writes to the FULL 320x528 VRAM, not our 160x264 virtual FB.
 *   2. It calls LCD_Refresh internally which would clash with fb_present().
 *
 * So we keep our own tiny 4x6 font and a printf-style overlay. The font is
 * small enough (96 glyphs * 4 bytes = 384 bytes) to live entirely in
 * .rodata and the draw routine is just a 6-iteration inner loop.
 *
 * This is intentionally NOT a full text console — it's the minimum needed
 * to show "fps / ticks / mode" labels over the demo.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "framebuffer.h"

/* Draw a single ASCII character (32..127) at pixel position (x, y) in the
 * virtual framebuffer. Characters outside the range are drawn as a space.
 * Color is RGB565. */
void overlay_char(int x, int y, char c, uint16_t color);

/* Draw a NUL-terminated string at (x, y). Returns the x position after the
 * last character (so callers can chain calls). */
int overlay_str(int x, int y, const char *s, uint16_t color);

/* Printf-style helper. Supports %s, %d, %u, %x, %c. Field width and
 * zero-padding supported for numeric types. */
void overlay_printf(int x, int y, uint16_t color, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
