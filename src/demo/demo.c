/*
 * cpcraft-port — demo/demo.c
 *
 * Small rendering demo built on the engine, to validate that the engine
 * hits the ~140k-tick frame budget the benchmark proved is achievable.
 *
 * The demo cycles between two scenes when the user presses the A (EXE) key:
 *
 *   Scene 0 — Plasma
 *     The classic 3-sin plasma, lifted from QBos07's benchmark verbatim.
 *     This is the heaviest per-pixel workload we expect to ship — if the
 *     engine can hold 30fps here, it can hold 30fps on anything reasonable
 *     we port from cpcraft.
 *
 *   Scene 1 — Textured cubes
 *     A 3x2 grid of textured rectangles, each showing a different
 *     procedural texture (checker / stone / grass). The texture offsets
 *     animate to prove the textured-rect path is fast enough for the
 *     floor/wall casters we'll need for cpcraft.
 *
 * On-screen overlay shows:
 *   - Scene name
 *   - Render ticks (the time spent rendering into the framebuffer)
 *   - Refresh ticks (the time spent blitting to the LCD — recorded by
 *     fb_present() and stored in fb_last_refresh_ticks)
 *   - Total ticks (render + refresh)
 *   - Frames-per-second, computed from total ticks
 *
 * Controls:
 *   D-pad     : move the camera offset (scene 1)
 *   EXE       : switch scene
 *   Backspace : toggle the overlay text on/off
 *   Shift+Clear : quit (same combo as CP-Raycaster-Demo)
 */
#include "demo.h"
#include "../engine/engine.h"
#include <sdk/os/debug.h>

/* ------------------------------------------------------------------ */
/*  Scene 0 — plasma (lifted from QBos07's benchmark)                 */
/* ------------------------------------------------------------------ */

/* Phase accumulators advanced per frame. 8.8 fixed point. */
static uint16_t plasma_frame_phase = 0;

static void render_plasma(uint16_t frame_phase)
{
    /* This is essentially the benchmark's render_plasma_line, but writing
     * into the 160x264 virtual framebuffer instead of directly to the LCD.
     *
     * For each pixel:
     *   s1 = sin(x_phase)
     *   s2 = sin(y_phase)
     *   s3 = sin((x_phase + y_phase) / 2)
     *   v  = (s1 + s2 + s3) / 4    -> 0..191
     *   color = RGB565(v & 0x1F, v & 0x3F, v & 0x1F)
     *
     * Each lookup is a single indexed load from sin_lut8, no multiplies. */
    const uint16_t x_step = 5 << 8;   /* horizontal frequency */

    for (int y = 0; y < FB_H; y++)
    {
        const uint16_t y_phase = (uint16_t)((y << 4) + frame_phase);
        const uint8_t  y_mod   = (uint8_t)(y_phase >> 8);

        uint16_t x_phase = frame_phase;
        uint16_t *dst = fb_vram + (y * FB_W);

        for (int x = 0; x < FB_W; x++)
        {
            const uint8_t s1 = sin_lut8[(uint8_t)(x_phase >> 8)];
            const uint8_t s2 = sin_lut8[y_mod];
            const uint8_t s3 = sin_lut8[(uint8_t)(((x_phase + y_phase) >> 9) & 0xFF)];

            const uint16_t sum = (uint16_t)(s1 + s2 + s3);   /* 0..765 */
            const uint8_t  v   = (uint8_t)(sum >> 2);         /* 0..191 */

            const uint16_t r = (uint16_t)(v & 0x1F) << 11;
            const uint16_t g = (uint16_t)(v & 0x3F) << 5;
            const uint16_t b = (uint16_t)(v & 0x1F);
            *dst++ = (uint16_t)(r | g | b);

            x_phase = (uint16_t)(x_phase + x_step);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Scene 1 — textured grid                                          */
/* ------------------------------------------------------------------ */

typedef struct {
    int x, y, w, h;
    const uint16_t (*tex)[TEX_SIZE];
    int u_off, v_off;
    int u_speed, v_speed;
} grid_cell_t;

static int cam_x = 0, cam_y = 0;   /* D-pad offset, applied to all cells */

static void render_textured_grid(int frame)
{
    /* Clear to a dark background so gaps between cells are visible. */
    fb_clear(0x0000);

    /* 3 columns x 2 rows of cells, each 48x48 in the virtual framebuffer.
     * Total: 144x96, centered horizontally. */
    const int cell_w = 48, cell_h = 48;
    const int cols = 3, rows = 2;
    const int total_w = cols * cell_w;
    const int origin_x = (FB_W - total_w) / 2 + cam_x;
    const int origin_y = 32 + cam_y;

    static const uint16_t (*tex_list[3])[TEX_SIZE] = {
        tex_checker, tex_stone, tex_grass
    };

    for (int row = 0; row < rows; row++)
        for (int col = 0; col < cols; col++)
        {
            const int idx = (row * cols + col) % 3;
            const int u_off = (frame * 1) & (TEX_SIZE - 1);
            const int v_off = (frame * 2 + idx * 5) & (TEX_SIZE - 1);

            fb_textured_rect(
                origin_x + col * cell_w,
                origin_y + row * cell_h,
                cell_w, cell_h,
                tex_list[idx],
                u_off, v_off);
        }

    /* Draw a frame around the grid so it's easy to see where the FB edge is. */
    const uint16_t frame_color = 0xFFFF;
    fb_hline(origin_x - 1, origin_x + total_w,     origin_y - 1,             frame_color);
    fb_hline(origin_x - 1, origin_x + total_w,     origin_y + rows*cell_h,   frame_color);
    fb_vline(origin_x - 1,           origin_y - 1, origin_y + rows*cell_h,   frame_color);
    fb_vline(origin_x + total_w,     origin_y - 1, origin_y + rows*cell_h,   frame_color);
}

/* ------------------------------------------------------------------ */
/*  Main demo loop                                                   */
/* ------------------------------------------------------------------ */

void demo_run(void)
{
    int scene = 0;          /* 0 = plasma, 1 = textured grid */
    int show_overlay = 1;
    int frame = 0;

    /* Colors for the overlay. */
    const uint16_t overlay_fg = 0xFFFF;
    const uint16_t overlay_bg = 0x0000;

    /* We use TMU channel 1 for render-tick measurement. fb_present() uses
     * the same channel for refresh-tick measurement, but it stops the TMU
     * when done, so we restart it for the next render pass. */
    POWER_MSTPCR0->s.TMU = 0;

    while (1)
    {
        /* --- Input -------------------------------------------------- */
        input_update();

        /* Shift+Clear = quit (same combo as CP-Raycaster-Demo). */
        if (input_exit_requested()) return;

        if (input_pressed(EK_EXE))       scene = (scene + 1) % 2;
        if (input_pressed(EK_BACKSPACE)) show_overlay = !show_overlay;

        if (input_down(EK_LEFT))  cam_x -= 2;
        if (input_down(EK_RIGHT)) cam_x += 2;
        if (input_down(EK_UP))    cam_y -= 2;
        if (input_down(EK_DOWN))  cam_y += 2;
        if (cam_x < -64) cam_x = -64;
        if (cam_x >  64) cam_x =  64;
        if (cam_y < -32) cam_y = -32;
        if (cam_y >  64) cam_y =  64;

        /* --- Render ------------------------------------------------- */
        /* Start TMU for render-tick measurement. */
        TMU_TCR_1->raw = 0;
        TMU_TCR_1->s.TPSC = PHI_DIV_4;
        *TMU_TCOR_1 = 0xFFFFFFFFu;
        *TMU_TCNT_1 = 0xFFFFFFFFu;
        TMU_TSTR->s.STR1 = 1;
        const uint32_t t_render_start = *TMU_TCNT_1;

        switch (scene)
        {
            case 0:
                render_plasma(plasma_frame_phase);
                plasma_frame_phase = (uint16_t)(plasma_frame_phase + 8);
                break;
            case 1:
                render_textured_grid(frame);
                break;
        }

        const uint32_t t_render_end = *TMU_TCNT_1;
        TMU_TSTR->s.STR1 = 0;
        const uint32_t render_ticks = t_render_start - t_render_end;

        /* --- Overlay (before present, so it's part of the render) --- */
        if (show_overlay)
        {
            /* Black background bar at the top so the text is readable. */
            fb_rect(0, 0, FB_W, 12, overlay_bg);

            const char *scene_name = (scene == 0) ? "PLASMA" : "TEXTURED";
            overlay_printf(1, 3, overlay_fg, "%s", scene_name);
            overlay_printf(60, 3, overlay_fg, "R%6u", render_ticks);
            overlay_printf(105, 3, overlay_fg, "B%6u", fb_last_refresh_ticks);
            overlay_printf(1, 3 + 7, overlay_fg, "F%4d", frame);

            /* FPS estimate: TMU_TICKS_PER_SEC / total_ticks. */
            const uint32_t total = render_ticks + fb_last_refresh_ticks;
            int fps = (total > 0) ? (int)(TMU_TICKS_PER_SEC / total) : 0;
            overlay_printf(60, 3 + 7, overlay_fg, "FPS%3d", fps);
        }

        /* --- Present to LCD ---------------------------------------- */
        fb_present();

        frame++;
    }
}
