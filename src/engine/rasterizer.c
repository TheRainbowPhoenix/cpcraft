/* src/engine/rasterizer.c */
/*
 * cpcraft-port — engine/rasterizer.c
 *
 * Z-buffered affine-textured quad rasterizer (16.16 fixed-point).
 *
 * Implementation:
 *   1. Find the min/max Y of the 4 projected vertices.
 *   2. For each scanline y in [minY, maxY]:
 *      a. Find the left and right edges by intersecting the quad's 4 edges
 *         with the scanline.
 *      b. Interpolate u, v, inv_z at the left and right intersection points.
 *      c. Walk [left_x, right_x] in 1-pixel steps, interpolating u, v, inv_z
 *         linearly across the span. Test inv_z against the z-buffer.
 *
 * Z-buffer encoding: we store a quantized 1/cz value. The caller passes
 * sz (camera-space depth, fix16). We compute inv_z = FIX16_ONE / sz and
 * store it as uint16_t (scaled). Larger = closer.
 *
 * All interpolation is fix16_t — no floats anywhere.
 */
#include "rasterizer.h"
#include "fix16.h"
#include <string.h>

uint16_t rz_zbuf[FB_W * FB_H];

/* Scale factor: inv_z (which is in fix16, so 1/1.0 = 65536) is multiplied
 * by this and shifted right 16 to get a uint16_t. For typical depths
 * (cz = 1..20 in fix16 = 65536..1310720), inv_z = 65536..3276, and
 * 3276 * 32 = 104832 which fits in uint16. */
#define INV_Z_SCALE  32

static inline uint16_t quantize_inv_z(fix16_t sz)
{
    if (sz <= 0) return 0;
    /* inv_z = 1 / sz = FIX16_ONE / sz (in fix16). */
    fix16_t inv_z = fix16_div(FIX16_ONE, sz);
    /* Scale to uint16: (inv_z * INV_Z_SCALE) >> 16. */
    int32_t q = (inv_z * INV_Z_SCALE) >> 16;
    if (q <= 0) return 0;
    if (q >= 65535) return 65535;
    return (uint16_t)q;
}

void rz_clear_zbuf(void)
{
    memset(rz_zbuf, 0, sizeof(rz_zbuf));
}

/* An edge of the quad, with fix16 interpolation values. */
struct Edge {
    int y_top, y_bot;
    fix16_t x_top, x_bot;
    fix16_t u_top, u_bot;
    fix16_t v_top, v_bot;
    fix16_t iz_top, iz_bot;
};

/* Build an edge between two vertices. Sorts so y_top <= y_bot. */
static struct Edge make_edge(const ScreenPoint *a, const ScreenPoint *b,
                             fix16_t ua, fix16_t va, fix16_t ub, fix16_t vb)
{
    struct Edge e;
    if (a->sy <= b->sy) {
        e.y_top = a->sy; e.y_bot = b->sy;
        e.x_top = fix16_from_int(a->sx); e.x_bot = fix16_from_int(b->sx);
        e.u_top = ua; e.u_bot = ub;
        e.v_top = va; e.v_bot = vb;
        e.iz_top = quantize_inv_z(a->sz);
        e.iz_bot = quantize_inv_z(b->sz);
    } else {
        e.y_top = b->sy; e.y_bot = a->sy;
        e.x_top = fix16_from_int(b->sx); e.x_bot = fix16_from_int(a->sx);
        e.u_top = ub; e.u_bot = ua;
        e.v_top = vb; e.v_bot = va;
        e.iz_top = quantize_inv_z(b->sz);
        e.iz_bot = quantize_inv_z(a->sz);
    }
    return e;
}

/* Sample an edge at scanline y. Returns true if y is in [y_top, y_bot]. */
static inline bool edge_sample(const struct Edge *e, int y,
                               fix16_t *x, fix16_t *u, fix16_t *v, uint16_t *iz)
{
    if (y < e->y_top || y > e->y_bot) return false;
    int dy = e->y_bot - e->y_top;
    if (dy == 0) {
        *x = e->x_top; *u = e->u_top; *v = e->v_top; *iz = e->iz_top;
        return true;
    }
    /* t = (y - y_top) / dy, in fix16. */
    fix16_t t = fix16_div(fix16_from_int(y - e->y_top), fix16_from_int(dy));
    *x = e->x_top + fix16_mul(e->x_bot - e->x_top, t);
    *u = e->u_top + fix16_mul(e->u_bot - e->u_top, t);
    *v = e->v_top + fix16_mul(e->v_bot - e->v_top, t);
    /* iz is uint16, so interpolate in fix16 and convert back. */
    fix16_t iz_f = fix16_from_int(e->iz_top) +
                   fix16_mul(fix16_from_int((int32_t)e->iz_bot - e->iz_top), t);
    *iz = (uint16_t)fix16_to_int(iz_f);
    return true;
}

void rz_draw_textured_quad(const ScreenPoint v[4],
                           const uint16_t tex[TEX_SIZE][TEX_SIZE])
{
    /* Reject if any vertex is behind the camera. */
    for (int i = 0; i < 4; i++)
        if (!v[i].visible) return;

    /* Texture coords for the 4 corners (counter-clockwise from top-left).
     * TEX_SIZE-1 in fix16 = (TEX_SIZE-1) << 16 = (TEX_SIZE-1) * FIX16_ONE. */
    #define FIX_TEX_MAX  ((TEX_SIZE - 1) * FIX16_ONE)
    static const fix16_t uvs[4][2] = {
        {0, 0},
        {FIX_TEX_MAX, 0},
        {FIX_TEX_MAX, FIX_TEX_MAX},
        {0, FIX_TEX_MAX},
    };
    #undef FIX_TEX_MAX

    /* Build the 4 edges. */
    struct Edge edges[4] = {
        make_edge(&v[0], &v[1], uvs[0][0], uvs[0][1], uvs[1][0], uvs[1][1]),
        make_edge(&v[1], &v[2], uvs[1][0], uvs[1][1], uvs[2][0], uvs[2][1]),
        make_edge(&v[2], &v[3], uvs[2][0], uvs[2][1], uvs[3][0], uvs[3][1]),
        make_edge(&v[3], &v[0], uvs[3][0], uvs[3][1], uvs[0][0], uvs[0][1]),
    };

    /* Find the Y range. */
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
        fix16_t lx = 0, lu = 0, lv = 0;
        uint16_t liz = 0;
        fix16_t rx = 0, ru = 0, rv = 0;
        uint16_t riz = 0;
        bool have = false;

        for (int i = 0; i < 4; i++)
        {
            fix16_t ex, eu, ev;
            uint16_t eiz;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &eiz)) continue;

            if (!have) {
                lx = rx = ex; lu = ru = eu; lv = rv = ev; liz = riz = eiz;
                have = true;
            } else {
                if (ex < lx) { lx = ex; lu = eu; lv = ev; liz = eiz; }
                if (ex > rx) { rx = ex; ru = eu; rv = ev; riz = eiz; }
            }
        }
        if (!have) continue;

        int x0 = fix16_to_int(lx);
        int x1 = fix16_to_int(rx);
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        fix16_t span = rx - lx;
        if (span < 1) span = 1;  /* avoid div-by-zero / tiny steps */

        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];

        for (int x = x0; x <= x1; x++)
        {
            /* t = (x - lx) / span, in fix16. */
            fix16_t t = fix16_div(fix16_from_int(x) - lx, span);

            uint16_t iz = liz + (uint16_t)fix16_to_int(
                fix16_mul(fix16_from_int((int32_t)riz - liz), t));
            if (iz < zbuf_row[x]) continue;
            zbuf_row[x] = iz;

            fix16_t u = lu + fix16_mul(ru - lu, t);
            fix16_t vv = lv + fix16_mul(rv - lv, t);

            int ti = fix16_to_int(u);
            int tj = fix16_to_int(vv);
            if (ti < 0) ti = 0;
            if (ti >= TEX_SIZE) ti = TEX_SIZE - 1;
            if (tj < 0) tj = 0;
            if (tj >= TEX_SIZE) tj = TEX_SIZE - 1;

            fb_row[x] = tex[tj][ti];
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
        fix16_t lx = 0, rx = 0;
        uint16_t liz = 0, riz = 0;
        bool have = false;

        for (int i = 0; i < 4; i++)
        {
            fix16_t ex, eu, ev;
            uint16_t eiz;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &eiz)) continue;
            if (!have) { lx = rx = ex; liz = riz = eiz; have = true; }
            else {
                if (ex < lx) { lx = ex; liz = eiz; }
                if (ex > rx) { rx = ex; riz = eiz; }
            }
        }
        if (!have) continue;

        int x0 = fix16_to_int(lx);
        int x1 = fix16_to_int(rx);
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        fix16_t span = rx - lx;
        if (span < 1) span = 1;

        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];
        for (int x = x0; x <= x1; x++)
        {
            fix16_t t = fix16_div(fix16_from_int(x) - lx, span);
            uint16_t iz = liz + (uint16_t)fix16_to_int(
                fix16_mul(fix16_from_int((int32_t)riz - liz), t));
            if (iz < zbuf_row[x]) continue;
            zbuf_row[x] = iz;
            fb_row[x] = color;
        }
    }
}
