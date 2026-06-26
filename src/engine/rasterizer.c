/* src/engine/rasterizer.c */
/*
 * cpcraft-port — engine/rasterizer.c
 *
 * Z-buffered affine-textured quad rasterizer.
 *
 * Implementation:
 *   1. Find the min/max Y of the 4 projected vertices.
 *   2. For each scanline y in [minY, maxY]:
 *      a. Find the left and right edges by intersecting the quad's 4 edges
 *         with the scanline. We get up to 2 intersection points; take the
 *         min X as left, max X as right.
 *      b. Interpolate u, v, inv_z at the left and right intersection points
 *         by linear interpolation along the edge from the top to bottom
 *         vertex of each edge.
 *      c. Walk [left_x, right_x] in 1-pixel steps, interpolating u, v, inv_z
 *         linearly across the span. Test inv_z against the z-buffer. If the
 *         pixel passes, fetch the texture pixel and write.
 *
 * This is the standard convex-polygon scanline rasterizer. Affine texture
 * mapping (linear interpolation of u and v in screen space) is wrong for
 * perspective, but it's fast and looks acceptable for cube faces viewed
 * at moderate angles.
 *
 * Z-buffer encoding: we store a quantized 1/cz value. To avoid per-pixel
 * divisions, the caller passes inv_z = 1/cz as a float, and we quantize
 * it to 16 bits via `qz = (uint16_t)(inv_z * 65536.0f * SCALE)` where
 * SCALE is chosen so that typical cube-face depths (cz ~ 1..20) map to
 * the full 16-bit range. We use SCALE = 32, so inv_z = 1/1 = 1.0 maps
 * to 65536*32 = clipped to 65535, and inv_z = 1/20 = 0.05 maps to 65536*0.05*32
 * ≈ 104. That gives us enough resolution to distinguish nearby faces.
 */
#include "rasterizer.h"
#include <math.h>
#include <string.h>

uint16_t rz_zbuf[FB_W * FB_H];

#define INV_Z_SCALE  32.0f

static inline uint16_t quantize_inv_z(float inv_z)
{
    float q = inv_z * 65536.0f * INV_Z_SCALE;
    if (q <= 0.0f) return 0;
    if (q >= 65535.0f) return 65535;
    return (uint16_t)q;
}

void rz_clear_zbuf(void)
{
    memset(rz_zbuf, 0, sizeof(rz_zbuf));
}

/* Interpolate a value at scanline y given two endpoints (y0, v0) and
 * (y1, v1). Returns v0 if y0 == y1. */
static inline float interp_edge(int y, int y0, float v0, int y1, float v1)
{
    if (y1 == y0) return v0;
    float t = (float)(y - y0) / (float)(y1 - y0);
    return v0 + (v1 - v0) * t;
}

/* An edge of the quad. */
struct Edge {
    int y_top, y_bot;
    float x_top, x_bot;
    float u_top, u_bot;
    float v_top, v_bot;
    float iz_top, iz_bot;
};

/* Build an edge between two vertices. Sorts so y_top <= y_bot. */
static struct Edge make_edge(const ScreenPoint *a, const ScreenPoint *b,
                             float ua, float va, float ub, float vb)
{
    struct Edge e;
    if (a->sy <= b->sy) {
        e.y_top = a->sy; e.y_bot = b->sy;
        e.x_top = (float)a->sx; e.x_bot = (float)b->sx;
        e.u_top = ua; e.u_bot = ub;
        e.v_top = va; e.v_bot = vb;
        e.iz_top = (a->sz > 0) ? 1.0f / a->sz : 0.0f;
        e.iz_bot = (b->sz > 0) ? 1.0f / b->sz : 0.0f;
    } else {
        e.y_top = b->sy; e.y_bot = a->sy;
        e.x_top = (float)b->sx; e.x_bot = (float)a->sx;
        e.u_top = ub; e.u_bot = ua;
        e.v_top = vb; e.v_bot = va;
        e.iz_top = (b->sz > 0) ? 1.0f / b->sz : 0.0f;
        e.iz_bot = (a->sz > 0) ? 1.0f / a->sz : 0.0f;
    }
    return e;
}

/* Sample an edge at scanline y. Returns true if y is in [y_top, y_bot]. */
static inline bool edge_sample(const struct Edge *e, int y,
                               float *x, float *u, float *v, float *iz)
{
    if (y < e->y_top || y > e->y_bot) return false;
    *x  = interp_edge(y, e->y_top, e->x_top,  e->y_bot, e->x_bot);
    *u  = interp_edge(y, e->y_top, e->u_top,  e->y_bot, e->u_bot);
    *v  = interp_edge(y, e->y_top, e->v_top,  e->y_bot, e->v_bot);
    *iz = interp_edge(y, e->y_top, e->iz_top, e->y_bot, e->iz_bot);
    return true;
}

void rz_draw_textured_quad(const ScreenPoint v[4],
                           const uint16_t tex[TEX_SIZE][TEX_SIZE])
{
    /* Reject if any vertex is behind the camera. (We don't clip — just
     * skip the whole face. This is fine for our use case.) */
    for (int i = 0; i < 4; i++)
        if (!v[i].visible) return;

    /* Texture coords for the 4 corners (counter-clockwise from top-left).
     * We assume the caller passed the vertices in the same order. */
    static const float uvs[4][2] = {
        {0.0f, 0.0f},
        {(float)(TEX_SIZE - 1), 0.0f},
        {(float)(TEX_SIZE - 1), (float)(TEX_SIZE - 1)},
        {0.0f, (float)(TEX_SIZE - 1)},
    };

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

    /* For each scanline, find left and right intersection. */
    for (int y = y_min; y <= y_max; y++)
    {
        float lx = 0, lu = 0, lv = 0, liz = 0;
        float rx = 0, ru = 0, rv = 0, riz = 0;
        bool have_l = false, have_r = false;

        for (int i = 0; i < 4; i++)
        {
            float ex, eu, ev, eiz;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &eiz)) continue;

            if (!have_l) {
                lx = ex; lu = eu; lv = ev; liz = eiz;
                rx = ex; ru = eu; rv = ev; riz = eiz;
                have_l = have_r = true;
            } else {
                if (ex < lx) { lx = ex; lu = eu; lv = ev; liz = eiz; }
                if (ex > rx) { rx = ex; ru = eu; rv = ev; riz = eiz; }
            }
        }
        if (!have_l) continue;

        int x0 = (int)lx, x1 = (int)rx;
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        /* Span width. */
        float span = rx - lx;
        if (span < 0.0001f) span = 0.0001f;

        /* Walk the span. */
        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];
        for (int x = x0; x <= x1; x++)
        {
            float t = ((float)x - lx) / span;
            float u = lu + (ru - lu) * t;
            float vv = lv + (rv - lv) * t;
            float iz = liz + (riz - liz) * t;

            uint16_t qz = quantize_inv_z(iz);
            if (qz < zbuf_row[x]) continue;
            zbuf_row[x] = qz;

            /* Nearest-neighbor texture sample. */
            int ti = (int)u;
            int tj = (int)vv;
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
        float lx = 0, rx = 0, liz = 0, riz = 0;
        bool have = false;

        for (int i = 0; i < 4; i++)
        {
            float ex, eu, ev, eiz;
            if (!edge_sample(&edges[i], y, &ex, &eu, &ev, &eiz)) continue;
            if (!have) { lx = rx = ex; liz = riz = eiz; have = true; }
            else {
                if (ex < lx) { lx = ex; liz = eiz; }
                if (ex > rx) { rx = ex; riz = eiz; }
            }
        }
        if (!have) continue;

        int x0 = (int)lx, x1 = (int)rx;
        if (x0 < 0) x0 = 0;
        if (x1 >= FB_W) x1 = FB_W - 1;
        if (x0 > x1) continue;

        float span = rx - lx;
        if (span < 0.0001f) span = 0.0001f;

        uint16_t *zbuf_row = &rz_zbuf[y * FB_W];
        uint16_t *fb_row = &fb_vram[y * FB_W];
        for (int x = x0; x <= x1; x++)
        {
            float t = ((float)x - lx) / span;
            float iz = liz + (riz - liz) * t;
            uint16_t qz = quantize_inv_z(iz);
            if (qz < zbuf_row[x]) continue;
            zbuf_row[x] = qz;
            fb_row[x] = color;
        }
    }
}
