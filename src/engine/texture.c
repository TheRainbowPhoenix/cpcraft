/* src/engine/texture.c */
/*
 * cpcraft-port — engine/texture.c
 *
 * Procedural texture generators + textured rectangle drawing.
 */
#include "texture.h"
#include "math_lut.h"

uint16_t tex_checker[TEX_SIZE][TEX_SIZE];
uint16_t tex_stone[TEX_SIZE][TEX_SIZE];
uint16_t tex_grass[TEX_SIZE][TEX_SIZE];
uint16_t tex_box[TEX_SIZE][TEX_SIZE];
uint16_t tex_wood[TEX_SIZE][TEX_SIZE];

/* Helper: pack RGB565. */
static inline uint16_t rgb565(int r, int g, int b)
{
    return (uint16_t)(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F));
}

void tex_init(void)
{
    /* Checkerboard: 8x8 cells of light/dark gray. */
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            int cell = ((x >> 2) + (y >> 2)) & 1;
            tex_checker[y][x] = cell ? rgb565(20, 20, 20) : rgb565(28, 28, 28);
        }

    /* Stone: noisy mid-gray, slight blue tint, darker "cracks". */
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            /* Hash-based noise: cheap pseudo-random per pixel. */
            uint32_t h = (uint32_t)(x * 73856093) ^ (uint32_t)(y * 19349663);
            h ^= h >> 13;
            h *= 0x85ebca6b;
            h ^= h >> 16;
            int n = (int)(h & 0x1F);

            int base = 24 + (n >> 1);   /* ~24..40 brightness */
            int r = base, g = base, b = base + 2;

            /* Cracks: every 4th row/col slightly darker. */
            if ((x & 3) == 0 || (y & 3) == 0) { r -= 8; g -= 8; b -= 8; }

            tex_stone[y][x] = rgb565(r, g, b);
        }

    /* Grass: green base with darker speckles. */
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            uint32_t h = (uint32_t)(x * 91136221) ^ (uint32_t)(y * 2654435761u);
            h ^= h >> 11;
            h *= 0x9e3779b1;
            h ^= h >> 15;
            int n = (int)(h & 0x07);

            int r = 6  + (n & 1);
            int g = 28 + (n >> 1);
            int b = 4;
            tex_grass[y][x] = rgb565(r, g, b);
        }

    /* Box: crate-style with dark border and lighter interior.
     * Looks like a Minecraft crate — brown wood with dark edges. */
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            int r, g, b;

            /* Dark border (2 pixels wide). */
            if (x < 2 || x >= TEX_SIZE - 2 || y < 2 || y >= TEX_SIZE - 2) {
                r = 10; g = 7; b = 3;  /* dark brown border */
            }
            /* Corner accents (slightly brighter). */
            else if ((x == 2 || x == TEX_SIZE - 3) && (y == 2 || y == TEX_SIZE - 3)) {
                r = 22; g = 16; b = 6;  /* corner highlight */
            }
            /* Interior: light wood color with grain. */
            else {
                /* Wood grain: darker horizontal lines every 4 px. */
                int grain = (y % 4 == 0) ? -4 : 0;
                r = 18 + grain;
                g = 13 + grain;
                b = 5 + grain;
            }

            tex_box[y][x] = rgb565(r, g, b);
        }

    /* Wood: plank-style texture with horizontal grain. */
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            int r, g, b;

            /* Plank separators (dark line every 4 rows). */
            if (y % 4 == 0) {
                r = 8; g = 5; b = 2;
            }
            else {
                /* Wood grain: slight noise along the plank. */
                uint32_t h = (uint32_t)(x * 73856093) ^ (uint32_t)(y * 19349663);
                h ^= h >> 13;
                int n = (int)(h & 0x03);
                r = 16 + n;
                g = 11 + n;
                b = 4;
            }

            tex_wood[y][x] = rgb565(r, g, b);
        }
}

void fb_textured_rect(int x, int y, int w, int h,
                      const uint16_t tex[TEX_SIZE][TEX_SIZE],
                      int u_offset, int v_offset)
{
    if (w <= 0 || h <= 0) return;

    /* Clip destination to framebuffer. */
    int x0 = x, y0 = y, x1 = x + w, y1 = y + h;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > FB_W) x1 = FB_W;
    if (y1 > FB_H) y1 = FB_H;
    if (x0 >= x1 || y0 >= y1) return;

    /* Precompute the texture-coordinate step as 16.16 fixed point so the
     * inner loop is one multiply + one shift per axis. */
    const uint32_t u_step = ((uint32_t)TEX_SIZE << 16) / (uint32_t)w;
    const uint32_t v_step = ((uint32_t)TEX_SIZE << 16) / (uint32_t)h;

    uint32_t v = (uint32_t)v_offset << 16;
    /* If we clipped the top, advance v past the clipped rows. */
    if (y0 > y) v += (uint32_t)(y0 - y) * v_step;

    for (int py = y0; py < y1; py++)
    {
        const int ty = (v >> 16) & (TEX_SIZE - 1);
        uint32_t u = (uint32_t)u_offset << 16;
        if (x0 > x) u += (uint32_t)(x0 - x) * u_step;

        const uint16_t *tex_row = tex[ty];
        uint16_t *dst = fb_vram + (py * FB_W) + x0;
        for (int px = x0; px < x1; px++)
        {
            const int tx = (u >> 16) & (TEX_SIZE - 1);
            *dst++ = tex_row[tx];
            u += u_step;
        }
        v += v_step;
    }
}
