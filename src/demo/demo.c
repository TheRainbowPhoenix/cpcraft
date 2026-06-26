/* src/demo/demo.c */
/*
 * cpcraft-port — demo/demo.c
 *
 * CPCraft prototype with scene cycling for hardware troubleshooting.
 *
 * Scenes (cycle with EXE):
 *   0 — CLEAR:     fill screen with a color. Tests fb_clear + fb_present.
 *   1 — PLASMA:    3-sin plasma effect. Tests fb_pixel + sin LUT.
 *   2 — TEXTURED:  3x2 grid of textured rects. Tests fb_textured_rect.
 *   3 — QUAD:      single projected textured quad. Tests camera + rasterizer.
 *   4 — SMALL:     small 8x8x8 voxel world. Tests world_render (few blocks).
 *   5 — WORLD:     full 64x32x64 voxel world. Tests everything.
 *
 * Each scene shows its name + tick counts via Debug_Printf so you can see
 * on hardware exactly where things break.
 *
 * Controls:
 *   EXE            : cycle to next scene
 *   D-pad          : look (scenes 3-5)
 *   Backspace(held): walk backward (scenes 4-5)
 *   Shift+Clear    : quit
 *
 * NO FLOATS. All math is 16.16 fixed-point (fix16_t).
 */
#include "demo.h"
#include "../engine/engine.h"
#include <sdk/os/debug.h>
#include <sdk/os/input.h>

/* ------------------------------------------------------------------ */
/*  Scene 0 — DIAGNOSTIC (print world block data)                      */
/* ------------------------------------------------------------------ */

static void scene_diag()
{
    fb_clear(0x0000);  /* black background */

    /* Print player position using overlay_printf (writes to fb_vram,
     * which gets streamed to the LCD by fb_present). Debug_Printf
     * would be overwritten by fb_present. */
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    int px = fix16_to_int(ex);
    int py = fix16_to_int(ey);
    int pz = fix16_to_int(ez);

    const uint16_t white = 0xFFFF;
    overlay_printf(1, 1, white, "P:%d,%d,%d", px, py, pz);
    overlay_printf(1, 8, white, "YAW:%d", player.yaw);

    /* Print blocks in front of the player (pz+1..pz+8). */
    for (int i = 0; i < 8; i++) {
        int bx = px;
        int bz = pz + 1 + i;
        if (bz >= WORLD_D) bz = WORLD_D - 1;
        int top_y = -1;
        uint8_t top_id = 0;
        for (int by = WORLD_H - 1; by >= 0; by--) {
            uint8_t id = world_get(bx, by, bz);
            if (id != BLK_AIR) { top_y = by; top_id = id; break; }
        }
        overlay_printf(1, 15 + i * 7, white, "[%d,%d]t=%d i=%d", bx, bz, top_y, top_id);
    }

    /* Print 3x3 grid around player. */
    overlay_printf(80, 1, white, "3x3 around P:");
    for (int dz = -1; dz <= 1; dz++) {
        for (int dx = -1; dx <= 1; dx++) {
            int bx = px + dx, bz = pz + dz;
            int top_y = -1;
            for (int by = WORLD_H - 1; by >= 0; by--) {
                if (world_get(bx, by, bz) != BLK_AIR) { top_y = by; break; }
            }
            overlay_printf(80, 8 + (dz + 1) * 7, white, "%d,%d=%d", dx, dz, top_y);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Scene 1 — PLASMA                                                   */
/* ------------------------------------------------------------------ */

static uint16_t plasma_phase = 0;

static void scene_plasma(void)
{
    const uint16_t x_step = 5 << 8;

    for (int y = 0; y < FB_H; y++)
    {
        const uint16_t y_phase = (uint16_t)((y << 4) + plasma_phase);
        const uint8_t  y_mod   = (uint8_t)(y_phase >> 8);

        uint16_t x_phase = plasma_phase;
        uint16_t *dst = fb_vram + (y * FB_W);

        for (int x = 0; x < FB_W; x++)
        {
            const uint8_t s1 = sin_lut8[(uint8_t)(x_phase >> 8)];
            const uint8_t s2 = sin_lut8[y_mod];
            const uint8_t s3 = sin_lut8[(uint8_t)(((x_phase + y_phase) >> 9) & 0xFF)];

            const uint16_t sum = (uint16_t)(s1 + s2 + s3);
            const uint8_t  v   = (uint8_t)(sum >> 2);

            const uint16_t r = (uint16_t)(v & 0x1F) << 11;
            const uint16_t g = (uint16_t)(v & 0x3F) << 5;
            const uint16_t b = (uint16_t)(v & 0x1F);
            *dst++ = (uint16_t)(r | g | b);

            x_phase = (uint16_t)(x_phase + x_step);
        }
    }
    plasma_phase = (uint16_t)(plasma_phase + 8);
}

/* ------------------------------------------------------------------ */
/*  Scene 2 — TEXTURED GRID                                            */
/* ------------------------------------------------------------------ */

static int cam_x = 0, cam_y = 0;

static void scene_textured(int frame)
{
    fb_clear(0x0000);

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

    const uint16_t fc = 0xFFFF;
    fb_hline(origin_x - 1, origin_x + total_w,   origin_y - 1,           fc);
    fb_hline(origin_x - 1, origin_x + total_w,   origin_y + rows*cell_h, fc);
    fb_vline(origin_x - 1,       origin_y - 1,   origin_y + rows*cell_h, fc);
    fb_vline(origin_x + total_w, origin_y - 1,   origin_y + rows*cell_h, fc);
}

/* ------------------------------------------------------------------ */
/*  Scene 3 — RAYCAST QUAD (a wall in front of the player)             */
/* ------------------------------------------------------------------ */

static void scene_quad(void)
{
    /* Use the raycaster to render the world. For this scene, the world
     * has already been generated, so the raycaster will show whatever
     * blocks are in front of the player. This tests the full raycast
     * pipeline. */
    fb_clear(0x6C59);  /* sky */

    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    raycast_render(ex, ey, ez, player.yaw, player.pitch);
}

/* ------------------------------------------------------------------ */
/*  Scene 4 — SMALL WORLD (raycaster, same as scene 3)                 */
/* ------------------------------------------------------------------ */

static void scene_small_world(void)
{
    fb_clear(0x6C59);  /* sky */
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    raycast_render(ex, ey, ez, player.yaw, player.pitch);
}

/* ------------------------------------------------------------------ */
/*  Scene 5 — FULL WORLD (raycaster)                                   */
/* ------------------------------------------------------------------ */

static void scene_full_world(void)
{
    fb_clear(0x6C59);  /* sky */
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    raycast_render(ex, ey, ez, player.yaw, player.pitch);
}

/* ------------------------------------------------------------------ */
/*  Main loop                                                          */
/* ------------------------------------------------------------------ */

static const char *scene_names[] = {
    "DIAG", "PLASMA", "TEXTURED", "RAYCAST", "RAYCAST", "RAYCAST"
};
#define SCENE_COUNT 6

void demo_run(void)
{
    int scene = 3;  /* Start on RAYCAST scene */
    int frame = 0;
    int fps = 0, fps_count = 0;
    uint32_t fps_timer = 0;

    POWER_MSTPCR0->s.TMU = 0;

    /* Show startup message. */
    Debug_Printf(0, 0, false, 0, "CPCraft-port starting...");
    fb_present();
    Debug_Printf(0, 1, false, 0, "engine_init done");

    while (1)
    {
        input_update();

        if (input_exit_requested()) return;

        /* Cycle scenes with EXE. */
        if (input_pressed(EK_EXE)) {
            scene = (scene + 1) % SCENE_COUNT;
            frame = 0;
        }

        /* Movement (scenes 3-5). */
        bool forward = input_down(EK_DOWN);   /* D-pad down = forward (towards look) */
        bool back    = input_down(EK_UP);     /* D-pad up = backward */
        bool jump    = false;
        (void)forward; (void)back; (void)jump;

        /* Look delta (BRAD per frame). 3°/frame ≈ 546 BRAD. */
        int16_t yaw_delta = 0, pitch_delta = 0;
        /* Use Left/Right for yaw, and Shift+Up/Down won't work since Shift
         * is exit... use number-less scheme: Left/Right = yaw. */
        if (input_down(EK_LEFT))  yaw_delta   -= 546;
        if (input_down(EK_RIGHT)) yaw_delta   += 546;
        /* For pitch, we'd need Up/Down but those are movement. Let's just
         * use Backspace for pitch down, EXE is scene cycle...
         * Actually for the prototype, let's keep it simple: Left/Right =
         * yaw, Up/Down = forward/back. Pitch is fixed at 0 for now. */

        /* Walk forward/back with Up/Down. */
        forward = input_down(EK_UP);
        back    = input_down(EK_DOWN);

        /* Update player (only matters for scenes 3-5). */
        player_update(forward, back, false, false, false,
                      yaw_delta, pitch_delta);

        /* --- Render the current scene --- */
        TMU_TCR_1->raw = 0;
        TMU_TCR_1->s.TPSC = PHI_DIV_4;
        *TMU_TCOR_1 = 0xFFFFFFFFu;
        *TMU_TCNT_1 = 0xFFFFFFFFu;
        TMU_TSTR->s.STR1 = 1;
        const uint32_t t_render_start = *TMU_TCNT_1;

        switch (scene)
        {
            case 0: scene_diag(); break;
            case 1: scene_plasma(); break;
            case 2: scene_textured(frame); break;
            case 3: scene_quad(); break;
            case 4: scene_small_world(); break;
            case 5: scene_full_world(); break;
        }

        const uint32_t t_render_end = *TMU_TCNT_1;
        TMU_TSTR->s.STR1 = 0;
        const uint32_t render_ticks = t_render_start - t_render_end;

        /* Draw HUD via our overlay (drawn into the framebuffer, so it
         * gets upscaled with the scene). */
        {
            const uint16_t fg = 0xFFFF, bg = 0x0000;
            fb_rect(0, 0, FB_W, 14, bg);
            overlay_printf(1, 3, fg, "S%d:%s", scene, scene_names[scene]);
            overlay_printf(55, 3, fg, "R%5lu", (unsigned long)render_ticks);
            overlay_printf(105, 3, fg, "B%5lu", (unsigned long)fb_last_refresh_ticks);

            fb_rect(0, FB_H - 14, FB_W, 14, bg);
            overlay_printf(1, FB_H - 11, fg, "F%4d", frame);
            overlay_printf(40, FB_H - 11, fg, "FPS%2d", fps);
            if (scene == 4) {
                overlay_printf(80, FB_H - 11, fg, "Y%3d", fix16_to_int(player.y));
            }
        }

        /* --- Present to LCD --- */
        fb_present();

        /* --- Hardware debug print (via Debug_Printf, writes to VRAM
         *     after fb_present, so it's drawn on top and visible even
         *     if the scene is broken). --- */
        Debug_Printf(0, 0, false, 0, "S%d:%s R%lu B%lu  ",
                     scene, scene_names[scene],
                     (unsigned long)render_ticks,
                     (unsigned long)fb_last_refresh_ticks);

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
