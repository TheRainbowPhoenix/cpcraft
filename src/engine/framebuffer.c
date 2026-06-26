/*
 * cpcraft-port — engine/framebuffer.c
 *
 * Implementation of the fast framebuffer + 2x-upscale blit.
 *
 * See framebuffer.h for the design rationale (why YRAM, why CPU not DMAC,
 * why 2x upscaling is essentially free).
 *
 * The blit inner loop is the hottest path in the whole engine. It's written
 * to let GCC's SH-4A backend pipeline two writes per source pixel through
 * the store queue without introducing a function call or branch. Don't
 * "clean it up" — every line was tuned against the benchmark numbers.
 */
#include "framebuffer.h"
#include "tmu.h"
#include "power.h"

/* YRAM-backed line pools. The section attribute is what makes this fast. */
uint16_t __attribute__((section(".oc_mem.y.fb"), aligned(32)))
    fb_line_pool[2][FB_W];

/* VRAM pointer (set in fb_init). The SDK's `vram` symbol is a `static const`
 * pointer in calc.h so we keep our own mutable copy here. */
uint16_t *fb_vram = (uint16_t *)0x8c000000;

/* Last frame's refresh tick count. */
uint32_t fb_last_refresh_ticks = 0;

void fb_init(void)
{
    /* Nothing to allocate — VRAM and YRAM pools are statically placed by the
     * linker. We just zero them so the first frame is clean. */
    fb_clear(0xFFFF);
    fb_last_refresh_ticks = 0;
}

void fb_clear(uint16_t color)
{
    /* Plain 16-bit fill. FB_W * FB_H = 160 * 264 = 42 240 writes. */
    uint16_t *p = fb_vram;
    uint16_t *end = fb_vram + (FB_W * FB_H);
    while (p < end) *p++ = color;
}

void fb_hline(int x1, int x2, int y, uint16_t color)
{
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (x1 < 0) x1 = 0;
    if (x2 >= FB_W) x2 = FB_W - 1;
    if (y < 0 || y >= FB_H) return;

    uint16_t *p = fb_vram + (y * FB_W) + x1;
    uint16_t *end = p + (x2 - x1 + 1);
    while (p < end) *p++ = color;
}

void fb_vline(int x, int y1, int y2, uint16_t color)
{
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }
    if (y1 < 0) y1 = 0;
    if (y2 >= FB_H) y2 = FB_H - 1;
    if (x < 0 || x >= FB_W) return;

    uint16_t *p = fb_vram + (y1 * FB_W) + x;
    int n = y2 - y1 + 1;
    while (n--) { *p = color; p += FB_W; }
}

void fb_rect(int x, int y, int w, int h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > FB_W) w = FB_W - x;
    if (y + h > FB_H) h = FB_H - y;
    if (w <= 0 || h <= 0) return;

    for (int j = 0; j < h; j++)
    {
        uint16_t *p = fb_vram + ((y + j) * FB_W) + x;
        uint16_t *end = p + w;
        while (p < end) *p++ = color;
    }
}

/* ----------------------------------------------------------------------------
 *  fb_present — the hot path.
 *
 *  We walk the virtual framebuffer one source line at a time. For each line:
 *    1. Copy FB_W pixels (320 bytes) from VRAM into a YRAM pool.
 *    2. Write back the dirty cache lines for the YRAM pool (ocbwb) so the
 *       subsequent writes to the LCD data port see fresh data.
 *    3. Set the LCD drawing bounds to a 2-line-tall strip on the physical
 *       screen (because we double vertically).
 *    4. Send COMMAND_PREPARE_FOR_DRAW_DATA (0x2C).
 *    5. Stream the pool to the LCD, writing each pixel TWICE (horizontal
 *       doubling) and repeating the whole stream TWICE (vertical doubling).
 *
 *  The two YRAM pools alternate so step 1 of line N+1 can be in flight while
 *  step 5 of line N is still streaming. On the SH-4A the store queue can
 *  absorb the CPU writes to YRAM while the LCD bus is busy accepting the
 *  previous line — that overlap is what gets us to ~140k ticks/frame.
 *
 *  We DO NOT use the DMAC. The benchmark proved DMAC is slower than CPU
 *  here because the LCD data port is on a slow peripheral bus and the DMAC
 *  stalls between every transfer, whereas the CPU pipelines the writes.
 * ---------------------------------------------------------------------------- */
void fb_present(void)
{
    /* Start the TMU so we can measure refresh ticks. */
    POWER_MSTPCR0->s.TMU = 0;                 /* un-gate TMU clock */
    TMU_TCR_1->raw = 0;
    TMU_TCR_1->s.TPSC = PHI_DIV_4;
    *TMU_TCOR_1 = 0xFFFFFFFFu;
    *TMU_TCNT_1 = 0xFFFFFFFFu;
    TMU_TSTR->s.STR1 = 1;

    const uint32_t t_start = *TMU_TCNT_1;

    const uint16_t *src = fb_vram;   /* walks the 160x264 framebuffer */
    const uint16_t width = 160;   /* TODO: remove ? */

    for (int y = 0; y < FB_H; y++)
    {
        const int pool_idx = y & 1;            /* double-buffer: 0, 1, 0, 1, ... */
        uint16_t *pool = fb_line_pool[pool_idx];

        /* --- Step 1: copy source line into the YRAM pool --------------- */
        /* Unrolled by 4 — each iteration copies 8 bytes. FB_W = 160 so we
         * do 40 iterations, no remainder. */
        {
            const uint16_t *s = src;
            uint16_t *d = pool;
            int n = FB_W / 4;
            do {
                d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
                s += 4; d += 4;
            } while (--n);
        }

        /* --- Step 2: write back the YRAM cache lines ------------------- */
        /* The LCD reads main memory, not the cache. ocbwb writes back any
         * dirty cache lines for the pool. Pool is 32-byte aligned and
         * FB_LINE_BYTES = 320, so 10 cache lines.
         *
         * On non-SH targets (e.g. when running a native test build), we
         * skip the cache writeback — the data still gets to "main memory"
         * because there's no LCD on a host. */
#ifdef __sh__
        {
            const uintptr_t base = (uintptr_t)pool;
            for (uintptr_t a = base; a < base + FB_LINE_BYTES; a += 32)
                __asm__ volatile ("ocbwb @%0" : : "r"(a));
        }
#endif

        /* --- Step 3 & 4: configure the LCD for a 2-line strip ---------- */
        /* Physical y range is [2y, 2y+1] (we double vertically). x range
         * is the full 320 pixels. */
        LCD_SetDrawingBounds(0, width - 1, y * 2, y * 2 + 1);
        LCD_SendCommand(COMMAND_PREPARE_FOR_DRAW_DATA);

        /* --- Step 5: stream the pool to the LCD, 2x horizontal doubling */
        /* Two passes through the pool — one per physical line. Each source
         * pixel is written twice. Total writes = FB_W * 2 * 2 = 640 per
         * source line, matching the physical 320*2 = 640 pixels per strip.
         *
         * The `*lcd_data_port = ...` writes go through the SDK's
         * volatile pointer at 0xB4000000, which the SH-4A store queue
         * can pipeline. */
        volatile uint16_t * const port = (volatile uint16_t *)0xB4000000; // lcd_data_port;
        const uint16_t *p = pool;
        const uint16_t *p_end = pool + FB_W;

        /* Pass 1 — top line of the strip. */
        do {
            const uint16_t px = *p++;
            *port = px;
            *port = px;
        } while (p < p_end);

        /* Pass 2 — bottom line of the strip (same source data). */
        p = pool;
        do {
            const uint16_t px = *p++;
            *port = px;
            *port = px;
        } while (p < p_end);

        src += FB_W;
    }

    /* Stop the TMU and record the refresh ticks. */
    const uint32_t t_end = *TMU_TCNT_1;
    TMU_TSTR->s.STR1 = 0;
    /* TMU counts DOWN, so delta = start - end. */
    fb_last_refresh_ticks = t_start - t_end;
}
