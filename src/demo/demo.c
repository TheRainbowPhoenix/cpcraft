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
/*  Scene 0 — CLEAR                                                    */
/* ------------------------------------------------------------------ */

static void scene_clear(int frame)
{
    /* Cycle through a few colors so we can see it's alive. */
    uint16_t colors[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0xFFFF};
    fb_clear(colors[(frame >> 4) & 3]);
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
/*  Scene 3 — SINGLE PROJECTED QUAD                                    */
/* ------------------------------------------------------------------ */

static void scene_quad(void)
{
    fb_clear(0x0000);
    rz_clear_zbuf();

    /* Place a single 2x2 block face in front of the player and project it. */
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);

    /* The quad's 4 corners in world space (a 2x2 square on the ground
     * 3 blocks in front of the player). */
    fix16_t bx = ex;
    fix16_t bz = ez + fix16_from_int(3);
    fix16_t by = fix16_from_int(0);

    ScreenPoint sp[4];
    sp[0] = camera_project(bx,           by,           bz,           ex, ey, ez, player.yaw, player.pitch);
    sp[1] = camera_project(bx+FIX16_ONE, by,           bz,           ex, ey, ez, player.yaw, player.pitch);
    sp[2] = camera_project(bx+FIX16_ONE, by+FIX16_ONE, bz,           ex, ey, ez, player.yaw, player.pitch);
    sp[3] = camera_project(bx,           by+FIX16_ONE, bz,           ex, ey, ez, player.yaw, player.pitch);

    rz_draw_textured_quad(sp, tex_checker);
}

/* ------------------------------------------------------------------ */
/*  Scene 4 — SMALL WORLD (8x8x8)                                      */
/* ------------------------------------------------------------------ */

/* We reuse the full world_render() but restrict to a small area by
 * temporarily changing the world bounds. Simplest: just render the
 * full world but with a small world. For the prototype, we just render
 * a subset by checking bx/bz range in a wrapper. */
static int small_render_count;

static void scene_small_world(void)
{
    fb_clear(0x6C59);  /* sky */
    rz_clear_zbuf();

    /* Render only blocks within 4 blocks of the player. */
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    int px = fix16_to_int(ex);
    int pz = fix16_to_int(ez);

    int x0 = px - 4, x1 = px + 4;
    int z0 = pz - 4, z1 = pz + 4;
    if (x0 < 0) x0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= WORLD_W) x1 = WORLD_W - 1;
    if (z1 >= WORLD_D) z1 = WORLD_D - 1;

    small_render_count = 0;

    /* Inline a restricted version of world_render. */
    const uint16_t yaw = player.yaw;
    const int16_t pitch = player.pitch;

    for (int by = 0; by < WORLD_H; by++)
        for (int bz = z0; bz <= z1; bz++)
            for (int bx = x0; bx <= x1; bx++)
            {
                uint8_t id = world_get(bx, by, bz);
                if (id == BLK_AIR) continue;
                small_render_count++;

                fix16_t cx = fix16_from_int(bx) + FIX16_HALF;
                fix16_t cy = fix16_from_int(by) + FIX16_HALF;
                fix16_t cz = fix16_from_int(bz) + FIX16_HALF;
                fix16_t vx = cx - ex, vy = cy - ey, vz = cz - ez;

                /* For each face, check exposure + backface cull + draw. */
                static const struct { int8_t dx,dy,dz; int8_t c[4][3]; int32_t n[3]; } faces[6] = {
                    { 1,0,0,{{1,0,1},{1,0,0},{1,1,0},{1,1,1}},{FIX16_ONE,0,0}},
                    {-1,0,0,{{0,0,0},{0,0,1},{0,1,1},{0,1,0}},{-FIX16_ONE,0,0}},
                    { 0,1,0,{{0,1,1},{1,1,1},{1,1,0},{0,1,0}},{0,FIX16_ONE,0}},
                    { 0,-1,0,{{0,0,0},{1,0,0},{1,0,1},{0,0,1}},{0,-FIX16_ONE,0}},
                    { 0,0,1,{{0,0,1},{1,0,1},{1,1,1},{0,1,1}},{0,0,FIX16_ONE}},
                    { 0,0,-1,{{1,0,0},{0,0,0},{0,1,0},{1,1,0}},{0,0,-FIX16_ONE}},
                };

                for (int f = 0; f < 6; f++)
                {
                    uint8_t nid = world_get(bx + faces[f].dx, by + faces[f].dy, bz + faces[f].dz);
                    if (!block_is_transparent(nid)) continue;
                    if (block_is_transparent(id) && nid == id) continue;

                    fix16_t dot = fix16_mul(faces[f].n[0], vx) +
                                  fix16_mul(faces[f].n[1], vy) +
                                  fix16_mul(faces[f].n[2], vz);
                    if (dot < 0) continue;

                    ScreenPoint sp[4];
                    for (int i = 0; i < 4; i++) {
                        sp[i] = camera_project(
                            fix16_from_int(bx) + fix16_from_int(faces[f].c[i][0]),
                            fix16_from_int(by) + fix16_from_int(faces[f].c[i][1]),
                            fix16_from_int(bz) + fix16_from_int(faces[f].c[i][2]),
                            ex, ey, ez, yaw, pitch);
                    }

                    const uint16_t (*tex)[TEX_SIZE];
                    if (f == 0)      tex = top_textures[id];
                    else if (f == 1) tex = bottom_textures[id];
                    else             tex = block_textures[id];
                    if (tex) rz_draw_textured_quad(sp, tex);
                }
            }
}

/* ------------------------------------------------------------------ */
/*  Scene 5 — FULL WORLD                                               */
/* ------------------------------------------------------------------ */

static void scene_full_world(void)
{
    fb_clear(0x6C59);  /* sky */
    rz_clear_zbuf();
    world_render();
}

/* ------------------------------------------------------------------ */
/*  Main loop                                                          */
/* ------------------------------------------------------------------ */

static const char *scene_names[] = {
    "CLEAR", "PLASMA", "TEXTURED", "QUAD", "SMALL", "WORLD"
};
#define SCENE_COUNT 6

void demo_run(void)
{
    int scene = 0;
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
            case 0: scene_clear(frame); break;
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
                overlay_printf(80, FB_H - 11, fg, "B%3d", small_render_count);
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
