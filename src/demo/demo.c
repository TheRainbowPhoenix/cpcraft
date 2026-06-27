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
/*  Scene 3 — SINGLE 3D CUBE (textured, turnable)                     */
/*                                                                     */
/*  Places a single cube in front of the camera and renders all 6     */
/*  faces using 3D projection + the z-buffered quad rasterizer.       */
/*  Backface culling removes faces pointing away from the camera.     */
/*  The player can turn the camera (Left/Right = yaw, Up/Down = pitch */
/*  and the cube's visible faces will change accordingly.             */
/* ------------------------------------------------------------------ */

/* Cube face definitions. Each face has 4 corners (CCW from outside)
 * and an outward normal. The corners are offsets from the cube's
 * min corner (bx, by, bz). */
struct CubeFace {
    int8_t corners[4][3];   /* 4 corner offsets, each (dx, dy, dz) in {0,1} */
    int32_t normal[3];      /* outward normal in fix16 */
};

static const struct CubeFace CUBE_FACES[6] = {
    /* +X face (east) */
    { {{1,0,1}, {1,0,0}, {1,1,0}, {1,1,1}}, {FIX16_ONE, 0, 0} },
    /* -X face (west) */
    { {{0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}}, {-FIX16_ONE, 0, 0} },
    /* +Y face (top) */
    { {{0,1,1}, {1,1,1}, {1,1,0}, {0,1,0}}, {0, FIX16_ONE, 0} },
    /* -Y face (bottom) */
    { {{0,0,0}, {1,0,0}, {1,0,1}, {0,0,1}}, {0, -FIX16_ONE, 0} },
    /* +Z face (south) */
    { {{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}}, {0, 0, FIX16_ONE} },
    /* -Z face (north) */
    { {{1,0,0}, {0,0,0}, {0,1,0}, {1,1,0}}, {0, 0, -FIX16_ONE} },
};

/* Draw a single cube at world position (bx, by, bz) with the given
 * textures for top/sides/bottom. Uses backface culling + z-buffer. */
static void draw_cube(int bx, int by, int bz,
                      const uint16_t (*top_tex)[TEX_SIZE],
                      const uint16_t (*side_tex)[TEX_SIZE],
                      const uint16_t (*bot_tex)[TEX_SIZE],
                      fix16_t ex, fix16_t ey, fix16_t ez,
                      uint16_t yaw, int16_t pitch)
{
    /* Cube center in fix16. */
    fix16_t cx = fix16_from_int(bx) + FIX16_HALF;
    fix16_t cy = fix16_from_int(by) + FIX16_HALF;
    fix16_t cz = fix16_from_int(bz) + FIX16_HALF;

    /* View vector (camera -> cube center). */
    fix16_t vx = cx - ex, vy = cy - ey, vz = cz - ez;

    for (int f = 0; f < 6; f++)
    {
        const struct CubeFace *face = &CUBE_FACES[f];

        /* Backface culling: the view vector goes FROM camera TO cube.
         * A face is visible if its normal points TOWARD the camera,
         * i.e. dot(normal, view) < 0.
         * If dot >= 0, the face points AWAY — skip it. */
        fix16_t dot = fix16_mul(face->normal[0], vx) +
                      fix16_mul(face->normal[1], vy) +
                      fix16_mul(face->normal[2], vz);
        if (dot >= 0) continue;

        /* Project the 4 corners. */
        ScreenPoint sp[4];
        for (int i = 0; i < 4; i++)
        {
            fix16_t wx = fix16_from_int(bx) + fix16_from_int(face->corners[i][0]);
            fix16_t wy = fix16_from_int(by) + fix16_from_int(face->corners[i][1]);
            fix16_t wz = fix16_from_int(bz) + fix16_from_int(face->corners[i][2]);
            camera_project(wx, wy, wz, ex, ey, ez, yaw, pitch, &sp[i]);
        }

        /* Skip if any vertex is behind the camera. */
        bool all_visible = true;
        for (int i = 0; i < 4; i++)
            if (!sp[i].visible) { all_visible = false; break; }
        if (!all_visible) continue;

        /* Pick texture for this face. */
        const uint16_t (*tex)[TEX_SIZE] = side_tex;
        if (f == 2 && top_tex)  tex = top_tex;
        if (f == 3 && bot_tex)  tex = bot_tex;

        if (tex)
            rz_draw_textured_quad(sp, tex);
    }
}

static void scene_cube(void)
{
    fb_clear(0x6C59);  /* sky blue */
    rz_clear_zbuf();

    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);

    /* Place the cube at a FIXED world position so the player can walk
     * around it and view it from all angles. */
    int bx = WORLD_W / 2;
    int bz = WORLD_D / 2 + 5;  /* a few blocks south of center */
    int by = 14;               /* at terrain height */

    draw_cube(bx, by, bz, tex_box, tex_box, tex_box,
              ex, ey, ez, player.yaw, player.pitch);
}

/* ------------------------------------------------------------------ */
/*  Scene 4 — DDA RAYCASTER (like CP-Raycaster-Demo)                  */
/* ------------------------------------------------------------------ */

/* Cast one ray per screen column through the voxel grid using DDA
 * (Amanatides & Woo algorithm). When we hit a solid block, draw a
 * textured vertical strip. Fill above with sky, below with floor.
 *
 * This is the same approach as CP-Raycaster-Demo but adapted for our
 * 3D voxel world (heightmap terrain). */

#define RAY_MAX_DIST  40

static void scene_raycast(void)
{
    fb_clear(0x6C59);  /* sky */

    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);

    int px = fix16_to_int(ex);
    int pz = fix16_to_int(ez);
    

    for (int dz = 2; dz <= 4; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int bx = px + dx;
            int bz = pz + dz;
            int by = 0;
            if (bx < 0 || bx >= WORLD_W) continue;
            if (bz < 0 || bz >= WORLD_D) continue;

            /* Use different textures for variety. */
            const uint16_t (*top)[TEX_SIZE] = (dx == 0) ? tex_grass : tex_checker;
            const uint16_t (*side)[TEX_SIZE] = (dz == 3) ? tex_stone : tex_checker;

            draw_cube(bx, by, bz, top, side, tex_stone,
                      ex, ey, ez, player.yaw, player.pitch);
        }
    }

    // /* FOV: 70 degrees. In BRAD: 70 * 65536/360 ≈ 12740.
    //  * Per-column step = FOV / FB_W ≈ 80 BRAD/pixel. */
    // const int32_t fov_step = (12740) / FB_W;

    // /* Horizon line: shifted by pitch. */
    // int horizon = FB_H / 2 - (player.pitch / 64);

    // for (int x = 0; x < FB_W; x++)
    //     {
    //     /* Ray angle = yaw + (x - FB_W/2) * fov_step (all in BRAD). */
    //     int32_t angle_offset = (x - FB_W / 2) * fov_step;
    //     uint16_t ray_angle = player.yaw + (uint16_t)angle_offset;

    //     fix16_t ray_x = fix16_sin_brads(ray_angle);
    //     fix16_t ray_z = fix16_cos_brads(ray_angle);

    //     /* DDA setup in the XZ plane. */
    //     int map_x = px;
    //     int map_z = pz;

    //     int step_x = (ray_x > 0) ? 1 : (ray_x < 0) ? -1 : 0;
    //     int step_z = (ray_z > 0) ? 1 : (ray_z < 0) ? -1 : 0;

    //     fix16_t frac_x = fix16_frac(ex);
    //     fix16_t frac_z = fix16_frac(ez);

    //     fix16_t t_max_x, t_max_z;
    //     fix16_t t_delta_x, t_delta_z;

    //     if (step_x > 0) {
    //         t_max_x = fix16_div(FIX16_ONE - frac_x, ray_x);
    //     } else if (step_x < 0) {
    //         t_max_x = fix16_div(frac_x, -ray_x);
    //     } else {
    //         t_max_x = 0x7FFFFFFF;
    //     }
    //     if (step_z > 0) {
    //         t_max_z = fix16_div(FIX16_ONE - frac_z, ray_z);
    //     } else if (step_z < 0) {
    //         t_max_z = fix16_div(frac_z, -ray_z);
    //     } else {
    //         t_max_z = 0x7FFFFFFF;
    //     }

    //     t_delta_x = (step_x != 0) ? fix16_div(FIX16_ONE, fix16_abs(ray_x)) : 0x7FFFFFFF;
    //     t_delta_z = (step_z != 0) ? fix16_div(FIX16_ONE, fix16_abs(ray_z)) : 0x7FFFFFFF;

    //     /* DDA march. */
    //     int hit_y = -1;
    //     uint8_t hit_id = BLK_AIR;
    //     int hit_side = 0;
    //     fix16_t hit_dist = 0;
    //     fix16_t hit_wall_x = 0;

    //     for (int step = 0; step < RAY_MAX_DIST * 4; step++)
    //     {
    //         if (t_max_x < t_max_z) {
    //             hit_dist = t_max_x;
    //             map_x += step_x;
    //             t_max_x += t_delta_x;
    //             hit_side = 0;
    //         } else {
    //             hit_dist = t_max_z;
    //             map_z += step_z;
    //             t_max_z += t_delta_z;
    //             hit_side = 1;
    //         }

    //         if ((unsigned)map_x >= WORLD_W || (unsigned)map_z >= WORLD_D)
    //             break;

    //         /* Find the highest non-air block in this column. */
    //         for (int by = WORLD_H - 1; by >= 0; by--) {
    //             uint8_t id = world_get(map_x, by, map_z);
    //             if (id != BLK_AIR && id != BLK_WATER) {
    //                 hit_y = by;
    //                 hit_id = id;
    //                 break;
    //             }
    //         }

    //         if (hit_y >= 0)
    //         {
    //             if (hit_side == 0) {
    //                 hit_wall_x = ez + fix16_mul(hit_dist, ray_z);
    //             } else {
    //                 hit_wall_x = ex + fix16_mul(hit_dist, ray_x);
    //             }
    //             hit_wall_x = fix16_frac(hit_wall_x);
    //             if (hit_wall_x < 0) hit_wall_x += FIX16_ONE;
    //             break;
    //         }
    //     }

    //     if (hit_y < 0)
    //         continue;  /* no hit — column is all sky */

    //     /* Compute wall strip height. */
    //     fix16_t dist = hit_dist;
    //     if (dist < FIX16_ONE) dist = FIX16_ONE;

    //     /* wall_height = FB_H / dist (in fix16). */
    //     fix16_t wall_h_fix = fix16_div(fix16_from_int(FB_H), dist);
    //     int wall_h = fix16_to_int(wall_h_fix);
    //     if (wall_h > FB_H) wall_h = FB_H;
    //     if (wall_h < 1) wall_h = 1;

    //     /* Wall position: top face at hit_y+1, bottom at hit_y. */
    //     fix16_t scale = wall_h_fix;
    //     fix16_t block_top_y = fix16_from_int(hit_y + 1);
    //     fix16_t block_bot_y = fix16_from_int(hit_y);

    //     int wall_top = horizon + fix16_to_int(fix16_mul(ey - block_top_y, scale));
    //     int wall_bot = horizon + fix16_to_int(fix16_mul(ey - block_bot_y, scale));

    //     int draw_top = wall_top;
    //     int draw_bot = wall_bot;
    //     if (draw_top < 0) draw_top = 0;
    //     if (draw_bot >= FB_H) draw_bot = FB_H - 1;

    //     /* Get texture. */
    //     const uint16_t (*tex)[TEX_SIZE] = block_textures[hit_id];
    //     if (!tex) tex = tex_stone;

    //     /* Texture U from hit_wall_x. */
    //     int tex_u = fix16_to_int(hit_wall_x * (TEX_SIZE - 1));
    //     if (tex_u < 0) tex_u = 0;
    //     if (tex_u >= TEX_SIZE) tex_u = TEX_SIZE - 1;

    //     /* Draw textured vertical strip. */
    //     for (int y = draw_top; y <= draw_bot; y++)
    //     {
    //         int tex_v = ((y - wall_top) * (TEX_SIZE - 1)) / (wall_h > 0 ? wall_h : 1);
    //         if (tex_v < 0) tex_v = 0;
    //         if (tex_v >= TEX_SIZE) tex_v = TEX_SIZE - 1;
    //         fb_pixel(x, y, tex[tex_v][tex_u]);
    //     }
    //     }
}

/* ------------------------------------------------------------------ */
/*  Scene 5 — FULL WORLD (3D projected cubes)                         */
/* ------------------------------------------------------------------ */

static void scene_world_3d(void)
{
    fb_clear(0x6C59);  /* sky */
    rz_clear_zbuf();

    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);

    /* Walk every block in the world and draw exposed faces.
     * This is the brute-force approach — for a 64x32x64 world that's
     * 131072 iterations. On hardware this will be slow, but it tests
     * the full 3D pipeline. */
    int px = fix16_to_int(ex);
    int pz = fix16_to_int(ez);

    /* Only render blocks within a small radius of the player to keep
     * the frame time reasonable. */
    int radius = 8;
    int x0 = px - radius, x1 = px + radius;
    int z0 = pz - radius, z1 = pz + radius;
    if (x0 < 0) x0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= WORLD_W) x1 = WORLD_W - 1;
    if (z1 >= WORLD_D) z1 = WORLD_D - 1;

    for (int by = 0; by < WORLD_H; by++)
        for (int bz = z0; bz <= z1; bz++)
            for (int bx = x0; bx <= x1; bx++)
            {
                uint8_t id = world_get(bx, by, bz);
                if (id == BLK_AIR) continue;

                /* Check if any neighbor is air (i.e., this block has
                 * at least one exposed face). If fully surrounded by
                 * solid blocks, skip it entirely. */
                bool exposed = false;
                if (!block_is_solid(world_get(bx+1, by, bz))) exposed = true;
                if (!block_is_solid(world_get(bx-1, by, bz))) exposed = true;
                if (!block_is_solid(world_get(bx, by+1, bz))) exposed = true;
                if (!block_is_solid(world_get(bx, by-1, bz))) exposed = true;
                if (!block_is_solid(world_get(bx, by, bz+1))) exposed = true;
                if (!block_is_solid(world_get(bx, by, bz-1))) exposed = true;
                if (!exposed) continue;

                /* Draw the cube with per-face neighbor checks. */
                for (int f = 0; f < 6; f++)
                {
                    const struct CubeFace *face = &CUBE_FACES[f];
                    int nx = bx + face->normal[0] / FIX16_ONE;
                    int ny = by + face->normal[1] / FIX16_ONE;
                    int nz = bz + face->normal[2] / FIX16_ONE;

                    /* Skip this face if the neighbor is solid. */
                    if (block_is_solid(world_get(nx, ny, nz))) continue;

                    /* Backface culling. */
                    fix16_t cx = fix16_from_int(bx) + FIX16_HALF;
                    fix16_t cy = fix16_from_int(by) + FIX16_HALF;
                    fix16_t cz = fix16_from_int(bz) + FIX16_HALF;
                    fix16_t vx = cx - ex, vy = cy - ey, vz = cz - ez;

                    fix16_t dot = fix16_mul(face->normal[0], vx) +
                                  fix16_mul(face->normal[1], vy) +
                                  fix16_mul(face->normal[2], vz);
                    if (dot >= 0) continue;

                    /* Project the 4 corners. */
                    ScreenPoint sp[4];
                    bool all_vis = true;
                    for (int i = 0; i < 4; i++)
                    {
                        fix16_t wx = fix16_from_int(bx) + fix16_from_int(face->corners[i][0]);
                        fix16_t wy = fix16_from_int(by) + fix16_from_int(face->corners[i][1]);
                        fix16_t wz = fix16_from_int(bz) + fix16_from_int(face->corners[i][2]);
                        camera_project(wx, wy, wz, ex, ey, ez, player.yaw, player.pitch, &sp[i]);
                        if (!sp[i].visible) all_vis = false;
                    }
                    if (!all_vis) continue;

                    /* Pick texture. */
                    const uint16_t (*tex)[TEX_SIZE] = block_textures[id];
                    if (f == 2 && top_textures[id])     tex = top_textures[id];
                    else if (f == 3 && bottom_textures[id]) tex = bottom_textures[id];

                    if (tex)
                        rz_draw_textured_quad(sp, tex);
                }
            }
}

/* ------------------------------------------------------------------ */
/*  Main loop                                                          */
/* ------------------------------------------------------------------ */

static const char *scene_names[] = {
    "DIAG", "PLASMA", "TEXTURED", "CUBE", "RAYCAST", "WORLD3D"
};
#define SCENE_COUNT 6

void demo_run(void)
{
    int scene = 3;  /* Start on CUBE scene */
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

        /* Controls (CPCraft-style numpad):
         *   8 = forward, 5 = backward, 4 = strafe left, 6 = strafe right
         *   2 = jump
         *   7 = camera left (yaw), 9 = camera right (yaw)
         *   D-pad up/down = look up/down (pitch)
         *   D-pad left/right = camera left/right (yaw) — same as 7/9
         *   EXE = cycle scene
         *   Shift+Clear = quit
         */
        bool forward  = input_down(EK_N8);
        bool back     = input_down(EK_N5);
        bool strafe_l = input_down(EK_N4);
        bool strafe_r = input_down(EK_N6);
        bool jump     = input_pressed(EK_N2);

        /* Look delta (BRAD per frame). 3°/frame ≈ 546 BRAD. */
        int16_t yaw_delta = 0, pitch_delta = 0;
        if (input_down(EK_N7) || input_down(EK_LEFT))  yaw_delta   -= 546;
        if (input_down(EK_N9) || input_down(EK_RIGHT)) yaw_delta   += 546;
        if (input_down(EK_UP))    pitch_delta -= 546;
        if (input_down(EK_DOWN))  pitch_delta += 546;

        /* Update player. */
        player_update(forward, back, strafe_l, strafe_r, jump,
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
            case 3: scene_cube(); break;
            case 4: scene_raycast(); break;
            case 5: scene_world_3d(); break;
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
