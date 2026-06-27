/* src/engine/player.c */
/*
 * cpcraft-port — engine/player.c
 *
 * Player movement + collision — PLAIN INT, no float, no fix16.
 *
 * Scale: 100 units per block. Sin/cos: 10000 = 1.0.
 * All math is plain int32_t multiplication and division.
 */
#include "player.h"
#include "math_lut.h"
#include "fix16.h"  /* for TRIG_SCALE */

Player player;

/* Player AABB: 60 units wide (0.6 blocks), 180 units tall (1.8 blocks). */
#define P_HW   30    /* half-width = 0.3 blocks = 30 units */
#define P_H    180   /* height = 1.8 blocks = 180 units */

/* Movement: 430 units/frame ≈ 4.3 blocks/sec at 30fps. */
#define MOVE_PER_FRAME  430

/* Gravity: 667 units/frame² ≈ 20 blocks/sec² at 30fps. */
#define GRAVITY_PER_FRAME  667

/* Jump velocity: 267 units/frame ≈ 8 blocks/sec at 30fps. */
#define JUMP_VELOCITY  267

/* Max fall speed: 2500 units/frame ≈ 25 blocks/sec. */
#define MAX_FALL  2500

/* Rotation: 546 BRAD per frame ≈ 3°/frame. */
#define YAW_PER_FRAME    546
#define PITCH_PER_FRAME  546
#define PITCH_MAX        16384   /* 90° in BRAD */

static bool collides(int32_t x, int32_t y, int32_t z)
{
    /* Convert to block coordinates (plain int division). */
    int x0 = (x - P_HW) / 100;
    int x1 = (x + P_HW) / 100;
    int y0 = y / 100;
    int y1 = (y + P_H) / 100;
    int z0 = (z - P_HW) / 100;
    int z1 = (z + P_HW) / 100;

    for (int by = y0; by <= y1; by++)
        for (int bz = z0; bz <= z1; bz++)
            for (int bx = x0; bx <= x1; bx++)
                if (block_is_solid(world_get(bx, by, bz)))
                    return true;
    return false;
}

void player_init(void)
{
    player.x = (WORLD_W / 2) * 100;
    player.z = (WORLD_D / 2) * 100;

    int bx = WORLD_W / 2;
    int bz = WORLD_D / 2;
    int by = WORLD_H - 1;
    while (by > 0 && world_get(bx, by, bz) == BLK_AIR) by--;
    player.y = (by + 3) * 100;  /* 3 blocks above terrain */

    player.yaw = 0;
    player.pitch = 0;
    player.vx = player.vy = player.vz = 0;
    player.on_ground = true;
}

void player_update(bool forward, bool back, bool left, bool right,
                   bool jump,
                   int16_t yaw_delta, int16_t pitch_delta)
{
    player.yaw   += yaw_delta;
    player.pitch += pitch_delta;

    if (player.pitch > PITCH_MAX)  player.pitch = PITCH_MAX;
    if (player.pitch < -PITCH_MAX) player.pitch = -PITCH_MAX;

    /* Build forward/right vectors in XZ plane (yaw only).
     * sin/cos return scale-10000. */
    int32_t sy = sin_brads(player.yaw);   /* -10000..10000 */
    int32_t cy = cos_brads(player.yaw);

    /* forward = (sin, 0, cos) — scale by MOVE_PER_FRAME / 10000. */
    int32_t fwd_x = sy * MOVE_PER_FRAME / TRIG_SCALE;
    int32_t fwd_z = cy * MOVE_PER_FRAME / TRIG_SCALE;
    /* right = (cos, 0, -sin). */
    int32_t right_x = cy * MOVE_PER_FRAME / TRIG_SCALE;
    int32_t right_z = -sy * MOVE_PER_FRAME / TRIG_SCALE;

    int32_t mx = 0, mz = 0;
    if (forward) { mx += fwd_x;   mz += fwd_z;   }
    if (back)    { mx -= fwd_x;   mz -= fwd_z;   }
    if (right)   { mx += right_x; mz += right_z; }
    if (left)    { mx -= right_x; mz -= right_z; }

    player.vx = mx;
    player.vz = mz;

    if (jump && player.on_ground) {
        player.vy = JUMP_VELOCITY;
        player.on_ground = false;
    }

    player.vy -= GRAVITY_PER_FRAME;
    if (player.vy < -MAX_FALL) player.vy = -MAX_FALL;

    /* Move per-axis with collision. */
    int32_t nx, ny, nz;

    nx = player.x + player.vx;
    if (!collides(nx, player.y, player.z)) player.x = nx;
    else player.vx = 0;

    nz = player.z + player.vz;
    if (!collides(player.x, player.y, nz)) player.z = nz;
    else player.vz = 0;

    ny = player.y + player.vy;
    if (!collides(player.x, ny, player.z)) {
        player.y = ny;
        player.on_ground = false;
    } else {
        if (player.vy <= 0) player.on_ground = true;
        player.vy = 0;
    }

    /* Keep inside world. */
    if (player.x < 100) player.x = 100;
    if (player.x > (WORLD_W - 2) * 100) player.x = (WORLD_W - 2) * 100;
    if (player.z < 100) player.z = 100;
    if (player.z > (WORLD_D - 2) * 100) player.z = (WORLD_D - 2) * 100;
    if (player.y < 0) { player.y = 0; player.vy = 0; }
}
