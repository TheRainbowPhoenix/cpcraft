/* src/engine/player.c */
/*
 * cpcraft-port — engine/player.c
 *
 * Player movement + collision (16.16 fixed-point, no floats).
 *
 * Movement model:
 *   - The player's yaw rotates the forward/right vectors in the XZ plane.
 *   - forward = (sin(yaw), 0, cos(yaw))  [yaw 0 = looking down +Z]
 *   - right   = (cos(yaw), 0, -sin(yaw))
 *   - Pressing forward moves the player along forward, etc.
 *   - Vertical movement is gravity + jump (no flying for now).
 *
 * Collision:
 *   - The player is an AABB 0.6 × 1.8 × 0.6 centered on (x, z) with
 *     feet at y.
 *   - We move one axis at a time and check the block bounds of the AABB
 *     against the world. If any block in the AABB is solid, we cancel
 *     the movement on that axis and zero the velocity.
 *
 * All math is fix16_t (16.16 fixed-point). The yaw is stored as BRAD
 * (binary radians, 0..65535 = 0..2π) for fast sin/cos lookups.
 */
#include "player.h"
#include "math_lut.h"
#include "fix16.h"

Player player;

/* Player AABB half-width in fix16 (0.3). */
#define P_HW   19661   /* 0.3 * 65536 */

/* Player height in fix16 (1.8). */
#define P_H    117965  /* 1.8 * 65536 */

/* Player eye offset in fix16 (1.6). */
#define P_EYE  104858  /* 1.6 * 65536 */

/* Movement speed: 4.3 blocks/sec. In fix16 per-frame at 30 FPS:
 * 4.3 / 30 = 0.1433 blocks/frame = 9386 fix16 units. */
#define MOVE_PER_FRAME  9386

/* Gravity: 20 blocks/sec^2. Per-frame at 30 FPS:
 * 20 / 900 = 0.0222 blocks/frame^2 = 1456 fix16 units. */
#define GRAVITY_PER_FRAME  1456

/* Jump velocity: 8 blocks/sec. Per-frame: 8/30 = 0.267 = 17476 fix16. */
#define JUMP_VELOCITY  17476

/* Max fall speed: 25 blocks/sec. Per-frame: 25/30 = 0.833 = 54613 fix16. */
#define MAX_FALL  54613

/* Rotation per frame (3 degrees). In BRAD: 3 * 65536/360 ≈ 546. */
#define YAW_PER_FRAME    546
#define PITCH_PER_FRAME  546
#define PITCH_MAX        (90 * 65536 / 360)   /* 90° in BRAD = 16384 */

/* Test whether the player's AABB at position (x, y, z) collides with any
 * solid block in the world. All inputs are fix16_t. */
static bool collides(fix16_t x, fix16_t y, fix16_t z)
{
    /* Block bounds of the AABB. */
    int x0 = fix16_to_int(x - P_HW);
    int x1 = fix16_to_int(x + P_HW);
    int y0 = fix16_to_int(y);
    int y1 = fix16_to_int(y + P_H);
    int z0 = fix16_to_int(z - P_HW);
    int z1 = fix16_to_int(z + P_HW);

    for (int by = y0; by <= y1; by++)
        for (int bz = z0; bz <= z1; bz++)
            for (int bx = x0; bx <= x1; bx++)
                if (block_is_solid(world_get(bx, by, bz)))
                    return true;
    return false;
}

void player_init(void)
{
    /* Spawn at the center of the world, on top of the terrain. */
    player.x = fix16_from_int(WORLD_W / 2);
    player.z = fix16_from_int(WORLD_D / 2);

    /* Find the top non-air block at (x, z) and put feet on top of it. */
    int bx = WORLD_W / 2;
    int bz = WORLD_D / 2;
    int by = WORLD_H - 1;
    while (by > 0 && world_get(bx, by, bz) == BLK_AIR) by--;
    player.y = fix16_from_int(by + 1);

    player.yaw   = 0;       /* looking down +Z */
    player.pitch = 0;       /* horizontal */
    player.vx = player.vy = player.vz = 0;
    player.on_ground = true;
}

void player_update(bool forward, bool back, bool left, bool right,
                   bool jump,
                   int16_t yaw_delta, int16_t pitch_delta)
{
    /* Apply rotation. */
    player.yaw   += yaw_delta;
    player.pitch += pitch_delta;

    /* Clamp pitch. */
    if (player.pitch > PITCH_MAX)  player.pitch = PITCH_MAX;
    if (player.pitch < -PITCH_MAX) player.pitch = -PITCH_MAX;

    /* Build forward / right vectors in the XZ plane (yaw only).
     * yaw 0 = +Z, yaw 90° (BRAD 16384) = +X. */
    fix16_t sy = fix16_sin_brads(player.yaw);
    fix16_t cy = fix16_cos_brads(player.yaw);

    /* forward = (sin(yaw), 0, cos(yaw)) */
    fix16_t fwd_x = sy;
    fix16_t fwd_z = cy;
    /* right = (cos(yaw), 0, -sin(yaw)) */
    fix16_t right_x = cy;
    fix16_t right_z = -sy;

    /* Horizontal movement intent. */
    fix16_t mx = 0, mz = 0;
    if (forward) { mx += fwd_x;   mz += fwd_z;   }
    if (back)    { mx -= fwd_x;   mz -= fwd_z;   }
    if (right)   { mx += right_x; mz += right_z; }
    if (left)    { mx -= right_x; mz -= right_z; }

    /* Normalize the horizontal intent (so diagonal isn't faster).
     * We use a cheap approximation: if |mx|+|mz| > 1.0, scale down
     * by the larger component. This avoids a sqrt. */
    fix16_t mag = fix16_abs(mx) + fix16_abs(mz);
    if (mag > FIX16_ONE) {
        mx = fix16_div(mx, mag);
        mz = fix16_div(mz, mag);
    }

    /* Set horizontal velocity (per-frame, already scaled). */
    player.vx = fix16_mul(mx, MOVE_PER_FRAME);
    player.vz = fix16_mul(mz, MOVE_PER_FRAME);

    /* Vertical: jump if on ground and jump requested. */
    if (jump && player.on_ground) {
        player.vy = JUMP_VELOCITY;
        player.on_ground = false;
    }

    /* Gravity. */
    player.vy -= GRAVITY_PER_FRAME;
    if (player.vy < -MAX_FALL) player.vy = -MAX_FALL;

    /* Try to move on each axis independently. */
    fix16_t nx, ny, nz;

    /* X axis. */
    nx = player.x + player.vx;
    if (!collides(nx, player.y, player.z))
        player.x = nx;
    else
        player.vx = 0;

    /* Z axis. */
    nz = player.z + player.vz;
    if (!collides(player.x, player.y, nz))
        player.z = nz;
    else
        player.vz = 0;

    /* Y axis. */
    ny = player.y + player.vy;
    if (!collides(player.x, ny, player.z)) {
        player.y = ny;
        player.on_ground = false;
    } else {
        if (player.vy <= 0) player.on_ground = true;
        player.vy = 0;
    }

    /* Keep the player inside the world. */
    if (player.x < FIX16_ONE) player.x = FIX16_ONE;
    if (player.x > fix16_from_int(WORLD_W - 2)) player.x = fix16_from_int(WORLD_W - 2);
    if (player.z < FIX16_ONE) player.z = FIX16_ONE;
    if (player.z > fix16_from_int(WORLD_D - 2)) player.z = fix16_from_int(WORLD_D - 2);
    if (player.y < 0) { player.y = 0; player.vy = 0; }
    }

void player_eye(fix16_t *ex, fix16_t *ey, fix16_t *ez) {
    *ex = player.x;
    *ey = player.y + P_EYE;
    *ez = player.z;
}
