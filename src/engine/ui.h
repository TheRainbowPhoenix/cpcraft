/* src/engine/ui.h */
/*
 * cpcraft-port — engine/ui.h
 *
 * Heads-up display: crosshair, hotbar, debug text.
 *
 * All UI is drawn in the virtual framebuffer's coordinate space
 * (FB_W × FB_H = 160 × 264), so it gets upscaled 2x by fb_present()
 * along with the world.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Draw a centered crosshair at the middle of the framebuffer. */
void ui_draw_crosshair(void);

/* Draw the hotbar at the bottom of the framebuffer.
 *
 *   selected  — index of the selected slot (0..8)
 *   slots     — array of 9 block IDs (BLK_*) in the hotbar
 *
 * The hotbar is drawn as 9 small squares with the block's color. */
void ui_draw_hotbar(int selected, const uint8_t slots[9]);

/* Draw the debug overlay (FPS, position, render ticks).
 *
 *   fps          — frames per second (0 to hide)
 *   render_ticks — TMU ticks spent in the render pass
 *   blit_ticks   — TMU ticks spent in fb_present()
 */
void ui_draw_debug(int fps, uint32_t render_ticks, uint32_t blit_ticks);

#ifdef __cplusplus
}
#endif
