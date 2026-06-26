/* src/engine/framebuffer.c */
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
#include <sdk/os/lcd.h>
#include "framebuffer.h"
#include "tmu.h"
#include "power.h"

#ifndef __sh__
    extern void sim_present(void);
#endif

/* VRAM pointer (set in fb_init). Initialized to the SDK's vram address
 * (0x8c000000 on hardware) at startup. In the simulator, fb_init() will
 * redirect this to LCD_GetVRAMAddress() which returns a real heap buffer. */
uint16_t *fb_vram = (uint16_t *)(uintptr_t)0x8c000000u;

/* Last frame's refresh tick count. */
uint32_t fb_last_refresh_ticks = 0;

void fb_init(void)
{
    /* On the simulator, redirect fb_vram to the SDK's VRAM buffer (which
     * is a real heap buffer, not the hardware's 0x8c000000). On hardware,
     * LCD_GetVRAMAddress() also returns 0x8c000000 so this is a no-op.
     *
     * We use #ifndef __sh__ so the hardware build skips the function call
     * entirely (saves a few cycles and avoids a dependency on the SDK
     * function pointer being initialized). */
#ifndef __sh__
    fb_vram = LCD_GetVRAMAddress();
#endif

    /* Zero the framebuffer so the first frame is clean. */
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
 *  Walk the 160x264 framebuffer one source line at a time. For each line:
 *    1. Set the LCD drawing bounds to a 2-line-tall strip (y*2, y*2+1).
 *    2. Send COMMAND_PREPARE_FOR_DRAW_DATA (0x2C).
 *    3. Stream the source line to the LCD twice (vertical doubling),
 *       writing each pixel twice (horizontal doubling).
 *
 *  We use per-line SetDrawingBounds because the SDK's LCD_SetDrawingBounds
 *  may not reliably handle a full-screen window in one shot (the OS
 *  implementation might reset state between calls). The per-line approach
 *  is what the original working version used and what the benchmark
 *  validated.
 *
 *  No YRAM intermediate, no ocbwb — we write directly from VRAM to the
 *  LCD data port. The CPU's store queue pipelines the writes.
 * ---------------------------------------------------------------------------- */
void fb_present(void)
{
    /* Start the TMU so we can measure refresh ticks. */
    #ifdef __sh__
    
    POWER_MSTPCR0->s.TMU = 0;                 /* un-gate TMU clock */
    TMU_TCR_1->raw = 0;
    TMU_TCR_1->s.TPSC = PHI_DIV_4;
    *TMU_TCOR_1 = 0xFFFFFFFFu;
    *TMU_TCNT_1 = 0xFFFFFFFFu;
    TMU_TSTR->s.STR1 = 1;

    const uint32_t t_start = *TMU_TCNT_1;
    #endif

   
    const uint16_t *src = fb_vram;
    for (int y = 0; y < FB_H; y++)
    {
        /* Set the LCD window for this 2-line strip. */
        LCD_SetDrawingBounds(0, LCD_W - 1, y * 2, y * 2 + 1);
        LCD_SendCommand(COMMAND_PREPARE_FOR_DRAW_DATA);

        /* Pass 1 — top physical line of the strip. */
        const uint16_t *p = src;
        const uint16_t *p_end = src + FB_W;
        do {
            const uint16_t px = *p++;
            SIM_LCD_WRITE(px);
            SIM_LCD_WRITE(px);
        } while (p < p_end);

        /* Pass 2 — bottom physical line (same source data). */
        p = src;
        do {
            const uint16_t px = *p++;
            SIM_LCD_WRITE(px);
            SIM_LCD_WRITE(px);
        } while (p < p_end);

        src += FB_W;
    }

#ifdef __sh__

    /* Stop the TMU and record the refresh ticks. */
    const uint32_t t_end = *TMU_TCNT_1;
    TMU_TSTR->s.STR1 = 0;
    /* TMU counts DOWN, so delta = start - end. */
    fb_last_refresh_ticks = t_start - t_end;
#else
    fb_last_refresh_ticks = 0;

    /* In the simulator, push the LCD framebuffer to the SDL window.
     * On hardware this is implicit — the LCD controller reads GRAM
     * continuously and displays it. In the simulator, sim_present()
     * updates the SDL texture from lcd_gram (which our SIM_LCD_WRITE
     * calls just filled) and pumps SDL events. */
    extern void sim_present(void);
    sim_present();
#endif

}
