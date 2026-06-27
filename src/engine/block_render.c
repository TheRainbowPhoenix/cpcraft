/* src/engine/block_render.c */
/*
 * cpcraft-port — engine/block_render.c
 *
 * Walk the world, find exposed faces, project + draw them.
 *
 * All positions are fix16_t (16.16 fixed-point). Block corners are
 * computed as fix16_from_int(bx) + offset (0 or FIX16_ONE).
 *
 * Face winding order (counter-clockwise when viewed from outside):
 *   +X face (right):  (1,0,1) (1,0,0) (1,1,0) (1,1,1)
 *   -X face (left):   (0,0,0) (0,0,1) (0,1,1) (0,1,0)
 *   +Y face (top):    (0,1,1) (1,1,1) (1,1,0) (0,1,0)
 *   -Y face (bottom): (0,0,0) (1,0,0) (1,0,1) (0,0,1)
 *   +Z face (south):  (0,0,1) (1,0,1) (1,1,1) (0,1,1)
 *   -Z face (north):  (1,0,0) (0,0,0) (0,1,0) (1,1,0)
 */
#include "block_render.h"
#include "rasterizer.h"
#include "camera.h"
#include "player.h"
#include "texture.h"
#include "fix16.h"

const uint16_t (*block_textures[BLK_COUNT])[TEX_SIZE];
const uint16_t (*top_textures[BLK_COUNT])[TEX_SIZE];
const uint16_t (*bottom_textures[BLK_COUNT])[TEX_SIZE];

void block_render_init(void)
{
    for (int i = 0; i < BLK_COUNT; i++) {
        block_textures[i]   = tex_stone;
        top_textures[i]     = tex_stone;
        bottom_textures[i]  = tex_stone;
    }
    top_textures[BLK_GRASS]   = tex_grass;
    block_textures[BLK_GRASS] = tex_grass;
    top_textures[BLK_DIRT]    = tex_stone;
    block_textures[BLK_DIRT]  = tex_stone;
    top_textures[BLK_WOOD]    = tex_stone;
    block_textures[BLK_WOOD]  = tex_checker;
    top_textures[BLK_LEAVES]  = tex_grass;
    block_textures[BLK_LEAVES]= tex_grass;
    top_textures[BLK_SAND]    = tex_checker;
    block_textures[BLK_SAND]  = tex_checker;
    top_textures[BLK_COBBLE]  = tex_stone;
    block_textures[BLK_COBBLE]= tex_stone;
    top_textures[BLK_PLANK]   = tex_checker;
    block_textures[BLK_PLANK] = tex_checker;
    top_textures[BLK_GLASS]   = tex_checker;
    block_textures[BLK_GLASS] = tex_checker;
    top_textures[BLK_WATER]   = tex_checker;
    block_textures[BLK_WATER] = tex_checker;
}

/* The 6 face directions. */
struct FaceDef {
    int8_t ndx, ndy, ndz;
    int8_t corners[4][3];
    int32_t normal_x, normal_y, normal_z;  /* fix16 */
};

static const struct FaceDef FACES[6] = {
    { 1, 0, 0,
      { {1,0,1}, {1,0,0}, {1,1,0}, {1,1,1} },
      FIX16_ONE, 0, 0 },
    { -1, 0, 0,
      { {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0} },
      -FIX16_ONE, 0, 0 },
    { 0, 1, 0,
      { {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0} },
      0, FIX16_ONE, 0 },
    { 0, -1, 0,
      { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} },
      0, -FIX16_ONE, 0 },
    { 0, 0, 1,
      { {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1} },
      0, 0, FIX16_ONE },
    { 0, 0, -1,
      { {1,0,0}, {0,0,0}, {0,1,0}, {1,1,0} },
      0, 0, -FIX16_ONE },
};

void world_render(void)
{
    fix16_t ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    const uint16_t yaw = player.yaw;
    const int16_t pitch = player.pitch;

    for (int by = 0; by < WORLD_H; by++)
        for (int bz = 0; bz < WORLD_D; bz++)
            for (int bx = 0; bx < WORLD_W; bx++)
            {
                uint8_t id = world_get(bx, by, bz);
                if (id == BLK_AIR) continue;

                /* Block center (fix16). */
                fix16_t cx = fix16_from_int(bx) + FIX16_HALF;
                fix16_t cy = fix16_from_int(by) + FIX16_HALF;
                fix16_t cz = fix16_from_int(bz) + FIX16_HALF;

                /* View vector (camera -> block center). */
                fix16_t vx = cx - ex, vy = cy - ey, vz = cz - ez;

                for (int f = 0; f < 6; f++)
                {
                    const struct FaceDef *fd = &FACES[f];

                    uint8_t nid = world_get(bx + fd->ndx, by + fd->ndy, bz + fd->ndz);
                    if (id == BLK_WATER) {
                        if (f != 2) continue;
                        if (nid != BLK_AIR) continue;
                    } else {
                        if (!block_is_transparent(nid)) continue;
                        if (block_is_transparent(id) && nid == id) continue;
                    }

                    /* Backface culling: dot(normal, view) > 0 = facing away.
                     * dot = nx*vx + ny*vy + nz*vz (all fix16). */
                    fix16_t dot = fix16_mul(fd->normal_x, vx) +
                                  fix16_mul(fd->normal_y, vy) +
                                  fix16_mul(fd->normal_z, vz);
                    if (dot < 0) continue;

                    /* Project the 4 corners. */
                    ScreenPoint sp[4];
                    for (int i = 0; i < 4; i++)
                    {
                        fix16_t wx = fix16_from_int(bx) + fix16_from_int(fd->corners[i][0]);
                        fix16_t wy = fix16_from_int(by) + fix16_from_int(fd->corners[i][1]);
                        fix16_t wz = fix16_from_int(bz) + fix16_from_int(fd->corners[i][2]);
                        sp[i] = camera_project(wx, wy, wz, ex, ey, ez, yaw, pitch);
                    }

                    const uint16_t (*tex)[TEX_SIZE];
                    if (f == 2)      tex = top_textures[id];
                    else if (f == 3) tex = bottom_textures[id];
                    else             tex = block_textures[id];

                    if (tex)
                        rz_draw_textured_quad(sp, tex);
                }
            }
}
