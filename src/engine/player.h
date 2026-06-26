/* src/engine/player.h */
/*
 * cpcraft-port — engine/player.h
 *
 * Player state: position, rotation, velocity, and AABB collision against
 * the voxel world.
 *
 * ALL math is 16.16 fixed-point (fix16_t). No floats — the ClassPad's
 * SH-4A has no FPU.
 *
 * Position is in fix16_t (16.16). The world is 64×32×64 blocks, so
 * positions fit easily in the integer part.
 *
 * Rotation:
 *   yaw   — BRAD (binary radians), uint16_t. 0 = +Z, 16384 = +X (90°).
 *   pitch — BRAD, int16_t. 0 = horizontal, ±16384 = ±90°.
 *
 * Collision: the player is an AABB 0.6 wide × 1.8 tall × 0.6 deep,
 * centered on (x, z) with feet at y. We test against world_get() at
 * the block bounds and reject movement if any block is solid.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "world.h"
#include "fix16.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    fix16_t x, y, z;        /* feet position (camera is at y + 1.6) */
    uint16_t yaw;           /* BRAD: 0 = +Z, 16384 = 90° right */
    int16_t  pitch;         /* BRAD: 0 = horizontal, +16384 = up */
    fix16_t vx, vy, vz;     /* velocity (fix16 per frame, not per sec) */
    bool  on_ground;     /* true if feet are touching a solid block */
} Player;

/* Global player state. */
extern Player player;

/* Initialize the player to a spawn position above the terrain. */
void player_init(void);

/* Update the player's velocity (gravity) and try to move, resolving
 * collisions per-axis.
 *
 * Movement is per-frame (not per-second) — the caller passes booleans
 * for forward/back/left/right/jump, and the function applies a fixed
 * delta per frame. This avoids needing real-time dt.
 *
 * yaw_delta / pitch_delta are in BRAD (binary radians) per frame. */
void player_update(bool forward, bool back, bool left, bool right,
                   bool jump,
                   int16_t yaw_delta, int16_t pitch_delta);

/* Get the player's eye position (where the camera sits). */
void player_eye(fix16_t *ex, fix16_t *ey, fix16_t *ez);

#ifdef __cplusplus
}
#endif
