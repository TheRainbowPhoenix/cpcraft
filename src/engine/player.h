/* src/engine/player.h */
/*
 * cpcraft-port — engine/player.h
 *
 * Player state — PLAIN INT, no float, no fix16.
 *
 * Scale: 100 units per block. So 5.12 blocks = 512 units.
 * Velocity: units per frame (at ~30fps).
 * Rotation: BRAD (uint16_t, 0..65535 = 0..2π).
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "world.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t x, y, z;        /* position in units (100 per block) */
    uint16_t yaw;           /* BRAD: 0 = +Z, 16384 = 90° right */
    int16_t  pitch;         /* BRAD: 0 = horizontal, +16384 = up */
    int32_t vx, vy, vz;     /* velocity (units per frame) */
    bool  on_ground;
} Player;

extern Player player;

void player_init(void);
void player_update(bool forward, bool back, bool left, bool right,
                   bool jump,
                   int16_t yaw_delta, int16_t pitch_delta);

/* Get the player's eye position (camera sits 160 units above feet). */
static inline void player_eye(int32_t *ex, int32_t *ey, int32_t *ez) {
    *ex = player.x;
    *ey = player.y + 160;  /* 1.6 blocks * 100 = 160 units */
    *ez = player.z;
}

#ifdef __cplusplus
}
#endif
