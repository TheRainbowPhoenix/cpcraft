/* src/engine/raycast.c */
/*
 * cpcraft-port — engine/raycast.c
 *
 * DDA voxel raycaster implementation.
 *
 * For each screen column, we cast a ray in the XZ plane and march
 * through the voxel grid using the Amanatides & Woo DDA algorithm.
 * When we hit a non-air block, we compute the wall height based on
 * distance and draw a textured vertical strip.
 *
 * The renderer is 2.5D: rays are cast in the XZ plane (no pitch
 * correction for now — pitch just shifts the horizon up/down).
 *
 * Texture mapping: for each wall hit, we compute the U coordinate
 * from where the ray hit the block face (perpendicular distance
 * modulo block size). V is interpolated along the vertical strip.
 *
 * Sky/floor: filled with solid colors by the caller (fb_clear).
 * The wall strip is drawn on top.
 */
#include "raycast.h"
#include "math_lut.h"
#include "world.h"
#include "block_render.h"
#include "fix16.h"

/* FOV: 70 degrees horizontal. Half-FOV in BRAD = 35 * 65536/360 ≈ 6370. */
#define FOV_HALF_BRADS  6370

/* Maximum ray march distance (in blocks). */
#define MAX_RAY_DIST  40

void raycast_render(fix16_t eye_x, fix16_t eye_y, fix16_t eye_z,
                    uint16_t yaw, int16_t pitch)
{
    /* Player's block position. */
    int px = fix16_to_int(eye_x);
    int pz = fix16_to_int(eye_z);

    /* FOV: 70 degrees horizontal. In BRAD: 70 * 65536 / 360 ≈ 12740.
     * Per-column angle step = FOV / FB_W = 12740 / 160 ≈ 80 BRAD/pixel.
     * This is a plain integer — BRAD is already a 16-bit angle format. */
    const int32_t fov_step = (FOV_HALF_BRADS * 2) / FB_W;  /* ≈ 79 BRAD/pixel */

    /* Horizon line: shifted by pitch. pitch is in BRAD (0 = horizontal).
     * We shift the horizon by pitch * FB_H / (FOV_VERTICAL * something).
     * For simplicity: horizon = FB_H/2 - pitch/64 (empirical scaling). */
    int horizon = FB_H / 2 - (pitch / 64);

    /* For each screen column... */
    for (int x = 0; x < FB_W; x++)
    {
        /* Ray angle = yaw + (x - FB_W/2) * fov_step (all in BRAD). */
        int32_t angle_offset = (x - FB_W / 2) * fov_step;
        uint16_t ray_angle = yaw + (uint16_t)angle_offset;

        /* Ray direction. */
        fix16_t ray_x = fix16_sin_brads(ray_angle);
        fix16_t ray_z = fix16_cos_brads(ray_angle);

        /* DDA setup. We march in the XZ plane.
         * The ray starts at (eye_x, eye_z) and goes in (ray_x, ray_z). */
        int map_x = px;
        int map_z = pz;

        /* Step direction. */
        int step_x = (ray_x > 0) ? 1 : (ray_x < 0) ? -1 : 0;
        int step_z = (ray_z > 0) ? 1 : (ray_z < 0) ? -1 : 0;

        /* Distance to the next voxel boundary on each axis.
         * If ray_x > 0: next boundary is at floor(x)+1, so t = (floor(x)+1 - x) / ray_x.
         * If ray_x < 0: next boundary is at floor(x), so t = (x - floor(x)) / -ray_x.
         * We use fix16 for the fractional part. */
        fix16_t frac_x = fix16_frac(eye_x);
        fix16_t frac_z = fix16_frac(eye_z);

        fix16_t t_max_x, t_max_z;
        fix16_t t_delta_x, t_delta_z;

        if (step_x > 0) {
            t_max_x = fix16_div(FIX16_ONE - frac_x, ray_x);
        } else if (step_x < 0) {
            t_max_x = fix16_div(frac_x, -ray_x);
        } else {
            t_max_x = 0x7FFFFFFF;  /* "infinity" */
        }

        if (step_z > 0) {
            t_max_z = fix16_div(FIX16_ONE - frac_z, ray_z);
        } else if (step_z < 0) {
            t_max_z = fix16_div(frac_z, -ray_z);
        } else {
            t_max_z = 0x7FFFFFFF;
        }

        /* t_delta = 1 / |ray_dir| per axis. */
        t_delta_x = (step_x != 0) ? fix16_div(FIX16_ONE, fix16_abs(ray_x)) : 0x7FFFFFFF;
        t_delta_z = (step_z != 0) ? fix16_div(FIX16_ONE, fix16_abs(ray_z)) : 0x7FFFFFFF;

        /* DDA march. */
        int hit_block = -1;
        int hit_side = 0;  /* 0 = X side, 1 = Z side */
        fix16_t hit_dist = 0;
        fix16_t hit_wall_x = 0;
        uint8_t hit_id = BLK_AIR;
        int hit_visible = 0;  /* 1 once we find a block whose wall is on-screen */

        for (int step = 0; step < MAX_RAY_DIST * 4 && !hit_visible; step++)
        {
            /* Advance to the next voxel. */
            if (t_max_x < t_max_z) {
                hit_dist = t_max_x;
                map_x += step_x;
                t_max_x += t_delta_x;
                hit_side = 0;
            } else {
                hit_dist = t_max_z;
                map_z += step_z;
                t_max_z += t_delta_z;
                hit_side = 1;
            }

            /* Check if out of bounds. */
            if ((unsigned)map_x >= WORLD_W || (unsigned)map_z >= WORLD_D)
                break;

            /* Check for a non-air block in this column. We scan from
             * the top down to find the highest non-air block. */
            int hit_y = -1;
            for (int by = WORLD_H - 1; by >= 0; by--) {
                uint8_t id = world_get(map_x, by, map_z);
                if (id != BLK_AIR && id != BLK_WATER) {
                    hit_y = by;
                    hit_id = id;
                    break;
                }
            }

            if (hit_y >= 0)
            {
                /* We hit a block. Check if its wall would be visible
                 * on screen. If not, keep marching to find a closer
                 * visible block.
                 *
                 * Quick visibility check: compute approximate wall_top
                 * and wall_bot. If both are off-screen, skip this block. */
                fix16_t dist = hit_dist;
                if (dist < FIX16_ONE) dist = FIX16_ONE;
                fix16_t wh = fix16_div(fix16_from_int(FB_H), dist);
                fix16_t scale = wh;
                fix16_t bt = fix16_from_int(hit_y + 1);
                fix16_t bb = fix16_from_int(hit_y);
                int approx_top = horizon + fix16_to_int(fix16_mul(eye_y - bt, scale));
                int approx_bot = horizon + fix16_to_int(fix16_mul(eye_y - bb, scale));

                /* If the wall is entirely above or below the screen,
                 * skip it and keep marching. */
                if (approx_bot < 0 || approx_top >= FB_H) {
                    /* Wall is off-screen. Continue marching. */
                    continue;
                }

                /* Wall is (at least partially) visible. */
                hit_block = hit_y;
                hit_visible = 1;

                /* Compute the wall X coordinate for texturing. */
                if (hit_side == 0) {
                    hit_wall_x = eye_z + fix16_mul(hit_dist, ray_z);
                } else {
                    hit_wall_x = eye_x + fix16_mul(hit_dist, ray_x);
                }
                hit_wall_x = fix16_frac(hit_wall_x);
                if (hit_wall_x < 0) hit_wall_x += FIX16_ONE;
            }
        }

        if (!hit_visible)
            continue;  /* no visible block — column is all sky */

        /* Compute the wall strip height.
         *
         * The wall is 1 block tall. At distance d, its screen height is:
         *   wall_h = FB_H * focal / d
         * where focal is the focal length in fix16.
         *
         * We use a simplified formula: wall_h = FB_H * 114 / d_int.
         * (114 ≈ 80 / tan(35°), the focal length for 70° FOV.)
         *
         * To avoid fix16_div in the hot loop, we precompute a lookup
         * table of 1/d for small d. But for now, just use fix16_div. */
        fix16_t dist = hit_dist;
        if (dist < FIX16_ONE) dist = FIX16_ONE;  /* clamp to avoid div by 0 */

        /* wall_height = (FB_H * focal) / dist
         * focal ≈ 114 in fix16 = 114 * 65536 = 7471104
         * FB_H = 264
         * So wall_height = 264 * 7471104 / dist = 1972354560 / dist
         * But that overflows int32! We need to be careful.
         *
         * Simplification: wall_height = FB_H / dist (in fix16).
         * dist is in fix16 (1.0 = 65536). So wall_height = FB_H * 65536 / dist.
         * = 264 * 65536 / dist = 17301504 / dist.
         * That fits in int32 (max 2^31 = 2147483648).
         */
        fix16_t wall_h_fix = fix16_div(fix16_from_int(FB_H), dist);
        int wall_h = fix16_to_int(wall_h_fix);
        if (wall_h > FB_H) wall_h = FB_H;
        if (wall_h < 1) wall_h = 1;

        /* Compute the wall strip position.
         *
         * The wall is 1 block tall. At distance d, 1 block appears
         * wall_h pixels tall on screen. The wall's top face is at
         * world Y = hit_block + 1, bottom at hit_block.
         *
         * screen_y = horizon + (eye_y - world_y) * scale
         *   where scale = wall_h (pixels per block at this distance).
         *   Positive (eye above block) → wall appears below horizon.
         *
         * So:
         *   wall_top = horizon + (eye_y - (hit_block + 1)) * scale
         *   wall_bot = horizon + (eye_y - hit_block) * scale
         */
        fix16_t scale = wall_h_fix;
        fix16_t block_top_y = fix16_from_int(hit_block + 1);
        fix16_t block_bot_y = fix16_from_int(hit_block);

        int wall_top = horizon + fix16_to_int(fix16_mul(eye_y - block_top_y, scale));
        int wall_bot = horizon + fix16_to_int(fix16_mul(eye_y - block_bot_y, scale));

        /* Clamp to screen. */
        int draw_top = wall_top;
        int draw_bot = wall_bot;
        if (draw_top < 0) draw_top = 0;
        if (draw_bot >= FB_H) draw_bot = FB_H - 1;

        /* Get the texture for this block. */
        const uint16_t (*tex)[TEX_SIZE] = block_textures[hit_id];
        if (!tex) tex = tex_stone;

        /* Compute texture U coordinate from hit_wall_x. */
        int tex_u = fix16_to_int(hit_wall_x * (TEX_SIZE - 1));
        if (tex_u < 0) tex_u = 0;
        if (tex_u >= TEX_SIZE) tex_u = TEX_SIZE - 1;

        /* Draw the textured vertical strip. */
        for (int y = draw_top; y <= draw_bot; y++)
        {
            /* Texture V: map [wall_top, wall_bot] to [0, TEX_SIZE-1]. */
            int tex_v = ((y - wall_top) * (TEX_SIZE - 1)) / (wall_h > 0 ? wall_h : 1);
            if (tex_v < 0) tex_v = 0;
            if (tex_v >= TEX_SIZE) tex_v = TEX_SIZE - 1;

            fb_pixel(x, y, tex[tex_v][tex_u]);
        }
    }
}
