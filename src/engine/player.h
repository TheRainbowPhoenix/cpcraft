/* src/engine/player.h */
/*
 * cpcraft-port — engine/player.h
 *
 * Player state: position, rotation, velocity, and AABB collision against
 * the voxel world.
 *
 * Position is in floating-point (the ClassPad's SH-4A has no FPU, but
 * we use software floats via the compiler's -m4-single-only / libgcc
 * soft-float helpers — slow but acceptable for one player update per
 * frame). CPCraft uses the same approach.
 *
 * Rotation is yaw (around Y) + pitch (around X), in degrees.
 *
 * Collision: the player is treated as an AABB 0.6 wide × 1.8 tall × 0.6
 * deep, centered on (x, z) with feet at y. We test against world_get()
 * at the 8 corners of the AABB and reject movement if any corner is
 * inside a solid block.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Player width (X and Z), height (Y), and eye offset from feet. */
#define PLAYER_W   0.6f
#define PLAYER_H   1.8f
#define PLAYER_EYE 1.6f

/* Movement speeds (blocks / second). */
#define PLAYER_MOVE_SPEED   4.3f   /* walking, like Minecraft */
#define PLAYER_ROT_SPEED    90.0f  /* degrees / second */
#define PLAYER_JUMP_SPEED   8.0f   /* initial jump velocity (blocks / second) */
#define GRAVITY             20.0f  /* blocks / second^2 */

typedef struct {
    float x, y, z;       /* feet position (camera is at y + PLAYER_EYE) */
    float yaw, pitch;    /* degrees; yaw 0 = +Z, pitch 0 = horizontal */
    float vx, vy, vz;    /* velocity (blocks / second) */
    bool  on_ground;     /* true if feet are touching a solid block */
} Player;

/* Global player state. */
extern Player player;

/* Initialize the player to a spawn position above the terrain. */
void player_init(void);

/* Update the player's velocity (gravity, friction) and try to move by
 * (vx*dt, vy*dt, vz*dt), resolving collisions per-axis.
 *
 * `dt` is in seconds. The engine's demo loop computes this from the TMU
 * tick delta. */
void player_update(float dt, bool forward, bool back, bool left, bool right,
                   bool jump, float yaw_delta, float pitch_delta);

/* Get the player's eye position (where the camera sits). */
static inline void player_eye(float *ex, float *ey, float *ez) {
    *ex = player.x; *ey = player.y + PLAYER_EYE; *ez = player.z;
}

#ifdef __cplusplus
}
#endif
