/* src/engine/rasterizer.c */
/*
 * cpcraft-port — engine/rasterizer.c
 *
 * Z-buffered affine-textured quad rasterizer — PLAIN INT version.
 *
 * NO fix16 in the hot loop. All interpolation uses plain int32_t
 * addition. This is crash-proof on SH-4A (no alignment issues, no
 * overflow, no 64-bit ops).
 *
 * Z-buffer: stores a uint16_t "1/z" value. Closer = larger.
 * We compute 1/z from the fix16_t sz field ONCE per edge endpoint
 * (not per pixel), then interpolate the uint16_t with integer math.
 */
#include "rasterizer.h"
#include <string.h>

uint16_t rz_zbuf[FB_W * FB_H];

/* Convert depth (int32_t 16.16) to uint16 z-buffer value.
 * Larger = closer. We use: zbuf_val = 65535 / (sz >> 8). */
static inline uint16_t depth_to_zbuf(int32_t sz)
{
    if (sz <= 0) return 0;
    uint32_t s = (uint32_t)sz >> 8;  /* sz / 256, as plain int */
    if (s == 0) s = 1;               /* clamp: very close */
    uint32_t val = 65535u / s;
    if (val > 65535) val = 65535;
    return (uint16_t)val;
}

void rz_clear_zbuf(void)
{
    memset(rz_zbuf, 0, sizeof(rz_zbuf));
}

/* An edge of the quad, using PLAIN INT for everything. */
struct Edge {
    int y_top, y_bot;
    int x_top, x_bot;       /* screen X at top/bottom */
    int u_top, u_bot;       /* texture U (scaled by 256) */
    int v_top, v_bot;       /* texture V (scaled by 256) */
    uint16_t z_top, z_bot;  /* z-buffer value at top/bottom */
};

static struct Edge make_edge(const ScreenPoint *a, const ScreenPoint *b,
                             int ua, int va, int ub, int vb)
{
    struct Edge e;
    if (a->sy <= b->sy) {
        e.y_top = a->sy; e.y_bot = b->sy;
        e.x_top = a->sx; e.x_bot = b->sx;
        e.u_top = ua; e.u_bot = ub;
        e.v_top = va; e.v_bot = vb;
        e.z_top = depth_to_zbuf(a->sz);
        e.z_bot = depth_to_zbuf(b->sz);
    } else {
        e.y_top = b->sy; e.y_bot = a->sy;
        e.x_top = b->sx; e.x_bot = a->sx;
        e.u_top = ub; e.u_bot = ua;
        e.v_top = vb; e.v_bot = va;
        e.z_top = depth_to_zbuf(b->sz);
        e.z_bot = depth_to_zbuf(a->sz);
    }
    return e;
}

/* Sample an edge at scanline y. Returns true if y is in range.
 * Outputs are PLAIN INT (no fix16). */
static inline bool edge_sample(const struct Edge *e, int y,
                               int *x, int *u, int *v, uint16_t *z)
{
    if (y < e->y_top || y > e->y_bot) return false;
    int dy = e->y_bot - e->y_top;
    if (dy == 0) {
        *x = e->x_top; *u = e->u_top; *v = e->v_top; *z = e->z_top;
        return true;
    }
    /* Integer interpolation: t = (y - y_top) / dy.
     * We compute everything as fractions with denominator dy. */
    int numer = y - e->y_top;
    *x = e->x_top + (e->x_bot - e->x_top) * numer / dy;
    *u = e->u_top + (e->u_bot - e->u_top) * numer / dy;
    *v = e->v_top + (e->v_bot - e->v_top) * numer / dy;
    /* Z interpolation: use plain int. */
    int z_val = e->z_top + (int)(e->z_bot - e->z_top) * numer / dy;
    if (z_val < 0) z_val = 0;
    if (z_val > 65535) z_val = 65535;
    *z = (uint16_t)z_val;
    return true;
}

void rz_draw_textured_quad(const ScreenPoint v[4],
                           const uint16_t tex[TEX_SIZE][TEX_SIZE])
{
    /* Reject if any vertex is behind the camera. */
    for (int i = 0; i < 4; i++)
        if (!v[i].visible) return;

    /* Texture coords for the 4 corners, scaled by 256 for integer
     * interpolation. TEX_SIZE-1 = 15, so max = 15*256 = 3840. */
    #define TEX_SCALE 256
    static const int uvs[4][2] = {
        {0, 0},
        {(TEX_SIZE-1) * TEX_SCALE, 0},
        {(TEX_SIZE-1) * TEX_SCALE, (TEX_SIZE-1) * TEX_SCALE},
        {0, (TEX_SIZE-1) * TEX_SCALE},
    };

    struct Edge edges[4] = {
        make_edge(&v[0], &v[1], uvs[0][0], uvs[0][1], uvs[1][0], uvs[1][1]),
        make_edge(&v[1], &v[2], uvs[1][0], uvs[1][1], uvs[2][0], uvs[2][1]),
        make_edge(&v[2], &v[3], uvs[2][0], uvs[2][1], uvs[3][0], uvs[3][1]),
        make_edge(&v[3], &v[0], uvs[3][0], uvs[3][1], uvs[0][0], uvs[0][1]),
    };

    int y_min = v[0].sy, y_max = v[0].sy;
    for (int i = 1; i < 4; i++) {
        if (v[i].sy < y_min) y_min = v[i].sy;
        if (v[i].sy > y_max) y_max = v[i].sy;
    }
    if (y_min < 0) y_min = 0;
    if (y_max >= FB_H) y_max = FB_H - 1;
    if (y_min > y_max) return;

    for (int y = y_min; y <= y_max; y++)
    {
        int lx = 0, lu = 0, lv = 0;
        uint16_t lz = 0;
        int rx = 0, ru = 0, rv = 0;
        uint16_t rz = 0;
        bool have = false;

        for (int i = 0; i < 4; i++)
        {
            int ex, eu, ev;
            uint16_t ez;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &ez)) continue;

            if (!have) {
                lx = rx = ex; lu = ru = eu; lv = rv = ev; lz = rz = ez;
                have = true;
            } else {
                if (ex < lx) { lx = ex; lu = eu; lv = ev; lz = ez; }
                if (ex > rx) { rx = ex; ru = eu; rv = ev; rz = ez; }
            }
        }
        if (!have) continue;

        int x0 = lx;
        int x1 = rx;
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        int span = rx - lx;
        if (span < 1) span = 1;

        /* Per-pixel deltas (integer, no fix16). */
        int du = (ru - lu) / span;
        int dv = (rv - lv) / span;
        int dz = ((int)rz - (int)lz) / span;

        /* Starting values at x0. */
        int frac = x0 - lx;
        int cu = lu + du * frac;
        int cv = lv + dv * frac;
        int cz = (int)lz + dz * frac;

        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];

        for (int x = x0; x <= x1; x++)
        {
            if (cz >= (int)zbuf_row[x])
            {
                zbuf_row[x] = (uint16_t)cz;

                /* Texture lookup: divide by TEX_SCALE. */
                int ti = cu / TEX_SCALE;
                int tj = cv / TEX_SCALE;
            if (ti < 0) ti = 0;
            if (ti >= TEX_SIZE) ti = TEX_SIZE - 1;
            if (tj < 0) tj = 0;
            if (tj >= TEX_SIZE) tj = TEX_SIZE - 1;

            fb_row[x] = tex[tj][ti];
        }
            cu += du;
            cv += dv;
            cz += dz;
        }
    }
}

void rz_draw_solid_quad(const ScreenPoint v[4], uint16_t color)
{
    for (int i = 0; i < 4; i++)
        if (!v[i].visible) return;

    struct Edge edges[4] = {
        make_edge(&v[0], &v[1], 0, 0, 0, 0),
        make_edge(&v[1], &v[2], 0, 0, 0, 0),
        make_edge(&v[2], &v[3], 0, 0, 0, 0),
        make_edge(&v[3], &v[0], 0, 0, 0, 0),
    };

    int y_min = v[0].sy, y_max = v[0].sy;
    for (int i = 1; i < 4; i++) {
        if (v[i].sy < y_min) y_min = v[i].sy;
        if (v[i].sy > y_max) y_max = v[i].sy;
    }
    if (y_min < 0) y_min = 0;
    if (y_max >= FB_H) y_max = FB_H - 1;
    if (y_min > y_max) return;

    for (int y = y_min; y <= y_max; y++)
    {
        int lx = 0, rx = 0;
        uint16_t lz = 0, rz = 0;
        bool have = false;

        for (int i = 0; i < 4; i++)
        {
            int ex, eu, ev;
            uint16_t ez;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &ez)) continue;
            if (!have) { lx = rx = ex; lz = rz = ez; have = true; }
            else {
                if (ex < lx) { lx = ex; lz = ez; }
                if (ex > rx) { rx = ex; rz = ez; }
            }
        }
        if (!have) continue;

        int x0 = lx;
        int x1 = rx;
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        int span = rx - lx;
        if (span < 1) span = 1;
        int dz = ((int)rz - (int)lz) / span;
        int frac = x0 - lx;
        int cz = (int)lz + dz * frac;

        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];
        for (int x = x0; x <= x1; x++)
        {
            if (cz >= (int)zbuf_row[x]) {
                zbuf_row[x] = (uint16_t)cz;
            fb_row[x] = color;
            }
            cz += dz;
        }
    }
}
