/* src/demo/demo.c */
/*
 * cpcraft-port — demo/demo.c
 *
 * Spinning triangle benchmark.
 *
 * Draws a filled triangle that grows/shrinks and shifts color every frame,
 * using direct framebuffer writes (fb_pixel). This tests raw pixel-fill
 * speed — no 3D projection, no z-buffer, no textures, just a tight loop.
 *
 * The triangle starts as a narrow peak at the top and widens toward the
 * bottom, exactly like the reference JS code. A per-frame "spin" is
 * achieved by offsetting the center column and triangle base, making it
 * sway left/right while pulsing in size.
 *
 * Controls:
 *   EXE       : cycle background color
 *   Shift+Clear : quit
 */
#include "demo.h"
#include "../engine/engine.h"
#include <sdk/os/debug.h>

void demo_run(void)
{
    int frame = 0;
    int fps = 0, fps_count = 0;
    uint32_t fps_timer = 0;
    uint16_t bg_colors[] = {0x0000, 0x001F, 0xF800, 0x07E0, 0xFFFF};
    int bg_idx = 0;

    POWER_MSTPCR0->s.TMU = 0;

    Debug_Printf(0, 0, false, 0, "Triangle benchmark starting...");
    fb_present();

    while (1)
    {
        input_update();
        if (input_exit_requested()) return;
        if (input_pressed(EK_EXE)) {
            bg_idx = (bg_idx + 1) % 5;
            frame = 0;
        }

        /* --- Render --- */
        TMU_TCR_1->raw = 0;
        TMU_TCR_1->s.TPSC = PHI_DIV_4;
        *TMU_TCOR_1 = 0xFFFFFFFFu;
        *TMU_TCNT_1 = 0xFFFFFFFFu;
        TMU_TSTR->s.STR1 = 1;
        const uint32_t t_start = *TMU_TCNT_1;

        /* Clear to background. */
        uint16_t bg = bg_colors[bg_idx];
        fb_clear(bg);

        /* --- Draw the spinning triangle ---
         *
         * The triangle is drawn row-by-row. Each row has a half-width
         * that grows from 0 (at the top) to some maximum (at the base).
         * The center column sways left/right based on frame count, and
         * the triangle height pulses to test different fill rates.
         *
         * Color math (from the reference):
         *   red   = ((row >> 2) & 0xF8) << 8   → R5 in bits 15..11
         *   green = ((col >> 1) & 0xFC)         → G6 in bits 10..5  (but shifted)
         *   blue  = ((row + col) >> 3) & 0x1F   → B5 in bits 4..0
         *
         * We build RGB565 directly and write it to fb_vram.
         */

        /* Triangle parameters that change each frame. */
        int sway = (frame * 3) % FB_W;          /* 0..159, wraps */
        int center_col = FB_W / 2 + (sway - FB_W / 2) / 2;  /* sway ±40 */
        if (center_col < 20) center_col = 20;
        if (center_col > FB_W - 20) center_col = FB_W - 20;

        /* Pulse the triangle height: 80..240 pixels. */
        int tri_height = 160 + ((frame * 2) % 80);
        if (tri_height > FB_H - 10) tri_height = FB_H - 10;

        /* Max half-width at the base. Scales with height. */
        int max_half = tri_height >> 3;  /* height/8, so base = height/4 wide */
        if (max_half < 2) max_half = 2;
        if (max_half > 60) max_half = 60;

        /* Draw row by row. */
        for (int row = 0; row < tri_height; row++)
        {
            /* Half-width at this row, linear from 0 to max_half. */
            int half_w = (row * max_half) / tri_height;
            if (half_w < 1) half_w = 1;

            int left = center_col - half_w;
            int right = center_col + half_w;

            /* Clip to framebuffer. */
            if (left < 0) left = 0;
            if (right >= FB_W) right = FB_W;
            if (left >= right) continue;

            /* Color for this row. */
            uint16_t r5 = ((row >> 2) & 0x1F);
            uint16_t g6_base = 0;  /* will vary per column */
            uint16_t b5_base = ((row) >> 3) & 0x1F;

            /* Write the span directly to fb_vram for speed. */
            uint16_t *dst = fb_vram + (row * FB_W) + left;
            for (int col = left; col < right; col++)
            {
                uint16_t g6 = ((col >> 1) & 0x3F);
                uint16_t b5 = ((row + col) >> 3) & 0x1F;

                *dst++ = (r5 << 11) | (g6 << 5) | b5;
            }
        }

        const uint32_t t_end = *TMU_TCNT_1;
        TMU_TSTR->s.STR1 = 0;
        const uint32_t render_ticks = t_start - t_end;

        /* --- HUD overlay --- */
        {
            const uint16_t fg = 0xFFFF, hud_bg = 0x0000;
            fb_rect(0, 0, FB_W, 14, hud_bg);
            overlay_printf(1, 3, fg, "TRI");
            overlay_printf(25, 3, fg, "R%5lu", (unsigned long)render_ticks);
            overlay_printf(65, 3, fg, "B%5lu", (unsigned long)fb_last_refresh_ticks);

            fb_rect(0, FB_H - 14, FB_W, 14, hud_bg);
            overlay_printf(1, FB_H - 11, fg, "F%4d", frame);
            overlay_printf(30, FB_H - 11, fg, "FPS%2d", fps);
            overlay_printf(60, FB_H - 11, fg, "H%3d", tri_height);
            overlay_printf(95, FB_H - 11, fg, "W%3d", max_half * 2);
        }

        /* --- Present --- */
        fb_present();

        /* --- Hardware debug print --- */
        // Debug_Printf(0, 0, false, 0, "TRI R%lu B%lu F%d FPS%d  ",
        //              (unsigned long)render_ticks,
        //              (unsigned long)fb_last_refresh_ticks,
        //              frame, fps);

        /* --- FPS counter --- */
        fps_count++;
        fps_timer += render_ticks + fb_last_refresh_ticks;
        if (fps_timer >= TMU_TICKS_PER_SEC) {
            fps = fps_count;
            fps_count = 0;
            fps_timer = 0;
        }

        frame++;
    }
}
