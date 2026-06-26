/* src/engine/player.c */
/*
 * cpcraft-port — engine/player.c
 *
 * Player movement + collision.
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
 *   - We move one axis at a time and check the 8 corners of the AABB
 *     against the world. If any corner is inside a solid block, we
 *     cancel the movement on that axis and zero the velocity.
 *
 * This is the simplest correct collision model — no stepping, no
 * sliding, no sloped surfaces. Good enough for a prototype.
 */
#include "player.h"
#include <math.h>

Player player;

/* Test whether the player's AABB at position (x, y, z) collides with any
 * solid block in the world. */
static bool collides(float x, float y, float z)
{
    /* AABB half-extents. */
    const float hw = PLAYER_W * 0.5f;

    /* Block bounds of the AABB. */
    int x0 = (int)floorf(x - hw);
    int x1 = (int)floorf(x + hw);
    int y0 = (int)floorf(y);
    int y1 = (int)floorf(y + PLAYER_H);
    int z0 = (int)floorf(z - hw);
    int z1 = (int)floorf(z + hw);

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
    player.x = WORLD_W * 0.5f;
    player.z = WORLD_D * 0.5f;

    /* Find the top non-air block at (x, z) and put feet on top of it. */
    int bx = (int)player.x;
    int bz = (int)player.z;
    int by = WORLD_H - 1;
    while (by > 0 && world_get(bx, by, bz) == BLK_AIR) by--;
    player.y = (float)(by + 1);

    player.yaw   = 0.0f;
    player.pitch = 0.0f;
    player.vx = player.vy = player.vz = 0.0f;
    player.on_ground = true;
}

void player_update(float dt, bool forward, bool back, bool left, bool right,
                   bool jump, float yaw_delta, float pitch_delta)
{
    /* Apply rotation. */
    player.yaw   += yaw_delta;
    player.pitch += pitch_delta;

    /* Clamp pitch to [-89, 89] to avoid gimbal-lock at the poles. */
    if (player.pitch >  89.0f) player.pitch =  89.0f;
    if (player.pitch < -89.0f) player.pitch = -89.0f;

    /* Wrap yaw to [0, 360). */
    while (player.yaw <    0.0f) player.yaw += 360.0f;
    while (player.yaw >= 360.0f) player.yaw -= 360.0f;

    /* Build forward / right vectors in the XZ plane (yaw only).
     * yaw 0 = +Z, yaw 90 = +X (right-handed). */
    const float yr = player.yaw * (3.14159265f / 180.0f);
    const float fwd_x = sinf(yr);
    const float fwd_z = cosf(yr);
    const float right_x = cosf(yr);
    const float right_z = -sinf(yr);

    /* Horizontal movement intent. */
    float mx = 0.0f, mz = 0.0f;
    if (forward) { mx += fwd_x;   mz += fwd_z;   }
    if (back)    { mx -= fwd_x;   mz -= fwd_z;   }
    if (right)   { mx += right_x; mz += right_z; }
    if (left)    { mx -= right_x; mz -= right_z; }

    /* Normalize the horizontal intent (so diagonal isn't faster). */
    float mlen = sqrtf(mx*mx + mz*mz);
    if (mlen > 0.001f) { mx /= mlen; mz /= mlen; }

    /* Set horizontal velocity directly (instant accel — arcade feel). */
    player.vx = mx * PLAYER_MOVE_SPEED;
    player.vz = mz * PLAYER_MOVE_SPEED;

    /* Vertical: jump if on ground and jump requested. */
    if (jump && player.on_ground)
    {
        player.vy = PLAYER_JUMP_SPEED;
        player.on_ground = false;
    }

    /* Gravity. */
    player.vy -= GRAVITY * dt;

    /* Try to move on each axis independently. If the move collides,
     * cancel that axis' velocity. */
    float nx, ny, nz;

    /* X axis. */
    nx = player.x + player.vx * dt;
    if (!collides(nx, player.y, player.z))
        player.x = nx;
    else
        player.vx = 0.0f;

    /* Z axis. */
    nz = player.z + player.vz * dt;
    if (!collides(player.x, player.y, nz))
        player.z = nz;
    else
        player.vz = 0.0f;

    /* Y axis. */
    ny = player.y + player.vy * dt;
    if (!collides(player.x, ny, player.z))
    {
        player.y = ny;
        player.on_ground = false;
    }
    else
    {
        /* If we were moving down, we hit the floor. */
        if (player.vy <= 0.0f)
            player.on_ground = true;
        player.vy = 0.0f;
    }

    /* Keep the player inside the world horizontally. */
    if (player.x < 1.0f) player.x = 1.0f;
    if (player.x > WORLD_W - 2.0f) player.x = WORLD_W - 2.0f;
    if (player.z < 1.0f) player.z = 1.0f;
    if (player.z > WORLD_D - 2.0f) player.z = WORLD_D - 2.0f;
    if (player.y < 0.0f) { player.y = 0.0f; player.vy = 0.0f; }
}
