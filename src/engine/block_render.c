/* src/engine/block_render.c */
/*
 * cpcraft-port — engine/block_render.c
 *
 * Walk the world, find exposed faces, project + draw them.
 *
 * Face winding order (counter-clockwise when viewed from outside):
 *
 *   +X face (right):  (1,0,1) (1,0,0) (1,1,0) (1,1,1)
 *   -X face (left):   (0,0,0) (0,0,1) (0,1,1) (0,1,0)
 *   +Y face (top):    (0,1,1) (1,1,1) (1,1,0) (0,1,0)
 *   -Y face (bottom): (0,0,0) (1,0,0) (1,0,1) (0,0,1)
 *   +Z face (south):  (0,0,1) (1,0,1) (1,1,1) (0,1,1)
 *   -Z face (north):  (1,0,0) (0,0,0) (0,1,0) (1,1,0)
 *
 * These windings produce outward-facing normals via the right-hand rule.
 *
 * Backface culling: we compute the geometric normal of each face (which
 * is one of ±X, ±Y, ±Z) and dot it with the vector from the camera to
 * the face center. If the dot is positive, the face points away and we
 * skip it. This cuts our draw count roughly in half.
 */
#include "block_render.h"
#include "rasterizer.h"
#include "camera.h"
#include "player.h"
#include "texture.h"

const uint16_t (*block_textures[BLK_COUNT])[TEX_SIZE];
const uint16_t (*top_textures[BLK_COUNT])[TEX_SIZE];
const uint16_t (*bottom_textures[BLK_COUNT])[TEX_SIZE];

void block_render_init(void)
{
    /* Map each block ID to its textures. For now we use the procedural
     * textures from texture.h:
     *   - stone/cobble/dirt/sand/plank -> tex_stone
     *   - grass top -> tex_grass, sides -> tex_grass (with dirt tint)
     *   - wood top/bottom -> tex_stone, sides -> tex_checker
     *   - leaves -> tex_grass
     *   - water -> tex_checker (blue-tinted)
     *   - glass -> tex_checker
     *
     * A real port would load actual Minecraft-format textures. */
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

/* The 6 face directions. Order: +X, -X, +Y, -Y, +Z, -Z.
 * Each entry is the (dx, dy, dz) of the neighbor to check for exposure,
 * plus the 4 corner offsets (CCW from outside) of the face. */
struct FaceDef {
    int8_t ndx, ndy, ndz;        /* neighbor offset */
    int8_t corners[4][3];        /* 4 corner offsets (CCW from outside) */
    float  normal[3];            /* outward normal */
};

static const struct FaceDef FACES[6] = {
    /* +X face. */
    { 1, 0, 0,
      { {1,0,1}, {1,0,0}, {1,1,0}, {1,1,1} },
      { 1.0f, 0.0f, 0.0f } },
    /* -X face. */
    { -1, 0, 0,
      { {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0} },
      { -1.0f, 0.0f, 0.0f } },
    /* +Y face (top). */
    { 0, 1, 0,
      { {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0} },
      { 0.0f, 1.0f, 0.0f } },
    /* -Y face (bottom). */
    { 0, -1, 0,
      { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} },
      { 0.0f, -1.0f, 0.0f } },
    /* +Z face (south). */
    { 0, 0, 1,
      { {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1} },
      { 0.0f, 0.0f, 1.0f } },
    /* -Z face (north). */
    { 0, 0, -1,
      { {1,0,0}, {0,0,0}, {0,1,0}, {1,1,0} },
      { 0.0f, 0.0f, -1.0f } },
};

void world_render(void)
{
    float ex, ey, ez;
    player_eye(&ex, &ey, &ez);
    const float yaw = player.yaw, pitch = player.pitch;

    /* Walk every block. */
    for (int by = 0; by < WORLD_H; by++)
        for (int bz = 0; bz < WORLD_D; bz++)
            for (int bx = 0; bx < WORLD_W; bx++)
            {
                uint8_t id = world_get(bx, by, bz);
                if (id == BLK_AIR) continue;

                /* Center of the block (used for backface culling). */
                float cx = (float)bx + 0.5f;
                float cy = (float)by + 0.5f;
                float cz = (float)bz + 0.5f;

                /* Vector from camera to block center. */
                float vx = cx - ex, vy = cy - ey, vz = cz - ez;

                /* For each of the 6 faces... */
                for (int f = 0; f < 6; f++)
                {
                    const struct FaceDef *fd = &FACES[f];

                    /* Is the neighbor air or transparent? If not, skip. */
                    uint8_t nid = world_get(bx + fd->ndx, by + fd->ndy, bz + fd->ndz);
                    if (id == BLK_WATER) {
                        /* Water only renders the top face if the block above is air. */
                        if (f != 2) continue;
                        if (nid != BLK_AIR) continue;
                    } else {
                        if (!block_is_transparent(nid)) continue;
                        /* Skip faces between two of the same transparent block
                         * (e.g. glass next to glass). */
                        if (block_is_transparent(id) && nid == id) continue;
                    }

                    /* Backface culling: skip if the face's normal points
                     * away from the camera. Dot(normal, view_vec) > 0
                     * means the face is pointing away. */
                    float dot = fd->normal[0]*vx + fd->normal[1]*vy + fd->normal[2]*vz;
                    if (dot < 0.0f) continue;

                    /* Compute the 4 world-space corners. */
                    ScreenPoint sp[4];
                    for (int i = 0; i < 4; i++)
                    {
                        float wx = (float)bx + fd->corners[i][0];
                        float wy = (float)by + fd->corners[i][1];
                        float wz = (float)bz + fd->corners[i][2];
                        sp[i] = camera_project(wx, wy, wz, ex, ey, ez, yaw, pitch);
                    }

                    /* Pick the texture for this face. */
                    const uint16_t (*tex)[TEX_SIZE];
                    if (f == 2)      tex = top_textures[id];
                    else if (f == 3) tex = bottom_textures[id];
                    else             tex = block_textures[id];

                    if (tex)
                        rz_draw_textured_quad(sp, tex);
                }
            }
}
