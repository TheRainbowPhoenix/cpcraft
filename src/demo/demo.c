/* src/demo/demo.c */
/*
 * cpcraft-port — demo/demo.c
 *
 * CPCraft prototype main loop.
 *
 * This is the "mini Minecraft" port: walk around a generated voxel world,
 * look around, place and break blocks. The world is rendered as textured
 * cube faces through the z-buffered rasterizer.
 *
 * Controls (matches CPCraft / CP-Raycaster-Demo conventions):
 *   D-pad up/down    : look up/down (pitch)
 *   D-pad left/right : turn left/right (yaw)
 *   EXE (hold)       : walk forward
 *   Backspace (hold) : walk backward
 *   EXE + Backspace  : jump (both held)
 *   1 / 3            : cycle hotbar selection (TODO: need number-key tracking)
 *   Backspace tap    : toggle debug overlay (TODO: edge detection)
 *   Shift+Clear : quit (same combo as CP-Raycaster-Demo)
 *
 * The control scheme is minimal for the prototype — we only track
 * D-pad + EXE + Backspace + Shift + Clear in the input module. Number
 * keys (1-9) for hotbar selection would require extending input.h to
 * track more KEYCODE_* values. That's a TODO for the next iteration.
 *
 * The on-screen HUD shows:
 *   - Crosshair at the screen center.
 *   - Hotbar at the bottom (9 slots, currently filled with a few block
 *     types for testing).
 *   - Debug overlay (top-left) with FPS + tick counts, toggled by
 *     Backspace.
 */
#include "demo.h"
#include "../engine/engine.h"
#include <sdk/os/debug.h>
#include <sdk/os/input.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  Hotbar                                                             */
/* ------------------------------------------------------------------ */

/* The blocks available in the hotbar. Slot 0 is empty-handed; the rest
 * are common block types. */
static uint8_t hotbar[9] = {
    BLK_STONE, BLK_GRASS, BLK_DIRT, BLK_WOOD, BLK_LEAVES,
    BLK_SAND,  BLK_COBBLE, BLK_PLANK, BLK_GLASS
};
static int hotbar_selected = 0;

/* ------------------------------------------------------------------ */
/*  Raycasting (for block selection)                                   */
/* ------------------------------------------------------------------ */

/* A fast voxel raycast (Amanatides & Woo's algorithm). Returns the first
 * non-air block hit by the ray from `origin` in direction `dir`, within
 * `max_dist` blocks. The hit position is stored in *out_x/y/z and the
 * face normal of the hit is stored in *out_normal. */
// static bool raycast_voxel(float ox, float oy, float oz,
//                           float dx, float dy, float dz,
//                           float max_dist,
//                           int *out_x, int *out_y, int *out_z,
//                           int *out_normal_x, int *out_normal_y, int *out_normal_z)
//     {
//     /* Normalize direction. */
//     float len = dx*dx + dy*dy + dz*dz;
//     if (len < 0.0001f) return false;
//     len = 1.0f / len;  /* sqrt would be more correct but this is fine
//                         * for raycasting — we just scale max_dist. */

//     int ix = (int)floorf(ox);
//     int iy = (int)floorf(oy);
//     int iz = (int)floorf(oz);

//     /* Step direction per axis. */
//     int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
//     int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
//     int step_z = (dz > 0) ? 1 : (dz < 0) ? -1 : 0;

//     /* Distance to the next voxel boundary on each axis. */
//     float t_max_x = (step_x != 0) ?
//         (((step_x > 0 ? (ix + 1) : ix) - ox) / dx) : 1e30f;
//     float t_max_y = (step_y != 0) ?
//         (((step_y > 0 ? (iy + 1) : iy) - oy) / dy) : 1e30f;
//     float t_max_z = (step_z != 0) ?
//         (((step_z > 0 ? (iz + 1) : iz) - oz) / dz) : 1e30f;

//     /* t_delta = distance along the ray per unit voxel step. */
//     float t_delta_x = (step_x != 0) ? fabsf(1.0f / dx) : 1e30f;
//     float t_delta_y = (step_y != 0) ? fabsf(1.0f / dy) : 1e30f;
//     float t_delta_z = (step_z != 0) ? fabsf(1.0f / dz) : 1e30f;

//     int normal_x = 0, normal_y = 0, normal_z = 0;
//     float t = 0.0f;

//     while (t <= max_dist * len)
// {
//         if (world_get(ix, iy, iz) != BLK_AIR &&
//             world_get(ix, iy, iz) != BLK_WATER)
//         {
//             *out_x = ix; *out_y = iy; *out_z = iz;
//             *out_normal_x = normal_x;
//             *out_normal_y = normal_y;
//             *out_normal_z = normal_z;
//             return true;
//         }

//         /* Advance to the next voxel. */
//         if (t_max_x < t_max_y && t_max_x < t_max_z) {
//             ix += step_x; t = t_max_x; t_max_x += t_delta_x;
//             normal_x = -step_x; normal_y = 0; normal_z = 0;
//         } else if (t_max_y < t_max_z) {
//             iy += step_y; t = t_max_y; t_max_y += t_delta_y;
//             normal_x = 0; normal_y = -step_y; normal_z = 0;
//         } else {
//             iz += step_z; t = t_max_z; t_max_z += t_delta_z;
//             normal_x = 0; normal_y = 0; normal_z = -step_z;
//         }
//     }
//     return false;
// }

/* Compute the player's view direction from yaw/pitch. */
// static void player_look(float *dx, float *dy, float *dz)
//         {
//     const float DEG2RAD = 3.14159265f / 180.0f;
//     float yr = player.yaw * DEG2RAD;
//     float pr = player.pitch * DEG2RAD;
//     float cp = cosf(pr), sp = sinf(pr);
//     float sy = sinf(yr), cy = cosf(yr);

//     /* Forward (yaw 0 = +Z, pitch 0 = horizontal, pitch +89 = up).
//      * forward = (sin(yaw)*cos(pitch), sin(pitch), cos(yaw)*cos(pitch)) */
//     *dx = sy * cp;
//     *dy = sp;
//     *dz = cy * cp;
//         }

/* ------------------------------------------------------------------ */
/*  Sky color                                                          */
/* ------------------------------------------------------------------ */

/* Light blue sky color (matches CPCraft's skyColors[8]). */
#define SKY_COLOR  0x6C59

/* ------------------------------------------------------------------ */
/*  Main game loop                                                     */
/* ------------------------------------------------------------------ */

void demo_run(void)
{
    bool show_debug = true;
    int frame = 0;

    /* FPS counter. */
    int fps = 0;
    int fps_frame_count = 0;
    uint32_t fps_timer = 0;

    /* We use TMU channel 1 for frame timing. fb_present() uses the same
     * channel for refresh-tick measurement, but it stops the TMU when
     * done, so we restart it for the next render pass. */
    POWER_MSTPCR0->s.TMU = 0;

    while (1)
    {
        /* --- Input -------------------------------------------------- */
        input_update();

        if (input_exit_requested()) return;

        /* Build the input state for player_update.
         *
         * Minimal scheme (see file header comment for rationale):
         *   D-pad = look (yaw/pitch)
         *   EXE (hold) = forward
         *   Backspace (hold) = backward
         *   EXE + Backspace (both held) = jump
         */
        const bool forward  = input_down(EK_EXE);
        const bool backward = input_down(EK_BACKSPACE);
        const bool jump     = forward && backward;

        /* Look delta. We target ~30 FPS, so per-frame delta =
         * PLAYER_ROT_SPEED * (1/30) = 3 degrees. */
        float yaw_delta = 0.0f, pitch_delta = 0.0f;
        if (input_down(EK_LEFT))  yaw_delta   -= 3.0f;
        if (input_down(EK_RIGHT)) yaw_delta   += 3.0f;
        if (input_down(EK_UP))    pitch_delta -= 3.0f;
        if (input_down(EK_DOWN))  pitch_delta += 3.0f;

        /* Update the player. We assume ~30 FPS (the engine doesn't have
         * a wall-clock timer yet — the TMU counts CPU ticks, not real
         * time, so we can't easily compute real dt). Fixed dt is fine
         * for a prototype. */
        const float dt = 1.0f / 30.0f;
        player_update(dt, forward, backward, false, false, jump,
                      yaw_delta, pitch_delta);

        /* --- Render ------------------------------------------------- */
        /* Start the TMU for render-tick measurement. */
        TMU_TCR_1->raw = 0;
        TMU_TCR_1->s.TPSC = PHI_DIV_4;
        *TMU_TCOR_1 = 0xFFFFFFFFu;
        *TMU_TCNT_1 = 0xFFFFFFFFu;
        TMU_TSTR->s.STR1 = 1;
        const uint32_t t_render_start = *TMU_TCNT_1;

        /* Clear the framebuffer to the sky color. */
        fb_clear(SKY_COLOR);

        /* Clear the z-buffer. */
        rz_clear_zbuf();

        /* Render the world. */
        world_render();

        /* Draw the HUD on top. */
        ui_draw_crosshair();
        ui_draw_hotbar(hotbar_selected, hotbar);

        const uint32_t t_render_end = *TMU_TCNT_1;
        TMU_TSTR->s.STR1 = 0;
        const uint32_t render_ticks = t_render_start - t_render_end;

        /* Toggle debug overlay. (We can't easily track the B key edge
         * here without more keys; let's just always show it for now.) */
        if (show_debug)
            ui_draw_debug(fps, render_ticks, fb_last_refresh_ticks);

        /* --- Present to LCD ---------------------------------------- */
        fb_present();

        /* --- FPS counter ------------------------------------------- */
        fps_frame_count++;
        fps_timer += render_ticks + fb_last_refresh_ticks;
        /* Update FPS once per ~1 second of TMU ticks. TMU runs at
         * TMU_TICKS_PER_SEC = 8 000 000 ticks/sec. */
        if (fps_timer >= TMU_TICKS_PER_SEC) {
            fps = fps_frame_count;
            fps_frame_count = 0;
            fps_timer = 0;
        }

        frame++;
    }
}
