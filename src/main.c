/* src/main.cpp */
/*
 * cpcraft-port — main.cpp
 *
 * Entry point. Initializes the engine, runs the demo, returns.
 *
 * The demo (demo/demo.c) is the only thing here that knows what to render.
 * main.cpp just wires it up and provides the App metadata that Hollyhock-3
 * uses to display the app in its launcher.
 *
 * SIMULATOR NOTE: When building for the simulator, this file is compiled
 * with -Dmain=app_main (set in the simulator's Makefile target) so the
 * simulator's main_simulator.cpp can call app_main() after initializing
 * SDL. On hardware, main() is the real entry point as usual.
 */
#include <appdef.h>

APP_NAME("CPLine")
APP_AUTHOR("CPLine")
APP_DESCRIPTION("Fast-render engine base + demo for the ClassPad")
APP_VERSION("0.0.1")


#include "./engine/framebuffer.h"
#include "./engine/tmu.h"
#include "./engine/power.h"

/* ------------------------------------------------------------------
 *  Tiny 3x5 bitmap font  (digits + F P S : space)
 * ------------------------------------------------------------------ */
#define FONT_W 3
#define FONT_H 5

static const uint8_t font3x5[][FONT_H] = {
    ['0'] = {0x7,0x5,0x5,0x5,0x7},
    ['1'] = {0x2,0x6,0x2,0x2,0x7},
    ['2'] = {0x7,0x1,0x7,0x4,0x7},
    ['3'] = {0x7,0x1,0x7,0x1,0x7},
    ['4'] = {0x5,0x5,0x7,0x1,0x1},
    ['5'] = {0x7,0x4,0x7,0x1,0x7},
    ['6'] = {0x7,0x4,0x7,0x5,0x7},
    ['7'] = {0x7,0x1,0x1,0x1,0x1},
    ['8'] = {0x7,0x5,0x7,0x5,0x7},
    ['9'] = {0x7,0x5,0x7,0x1,0x7},
    ['F'] = {0x7,0x4,0x6,0x4,0x4},
    ['P'] = {0x7,0x5,0x7,0x4,0x4},
    ['S'] = {0x7,0x4,0x7,0x1,0x7},
    [':'] = {0x0,0x2,0x0,0x2,0x0},
    [' '] = {0x0,0x0,0x0,0x0,0x0},
};

static void draw_char(int x, int y, char ch, uint16_t color)
{
    if ((unsigned)ch >= sizeof(font3x5) / sizeof(font3x5[0])) return;
    const uint8_t *g = font3x5[(unsigned char)ch];
    for (int r = 0; r < FONT_H; r++) {
        uint8_t row = g[r];
        for (int c = 0; c < FONT_W; c++)
            if (row & (4 >> c))
                fb_pixel(x + c, y + r, color);
    }
}

static void draw_text(int x, int y, const char *s, uint16_t color)
{
    while (*s) {
        draw_char(x, y, *s, color);
        x += FONT_W + 1;
        s++;
    }
}

static void draw_num(int x, int y, int val, uint16_t color)
{
    char buf[8];
    int n = 0;
    if (val == 0) {
        buf[n++] = '0';
    } else {
        while (val > 0 && n < 8) {
            buf[n++] = '0' + (val % 10);
            val /= 10;
        }
    }
    while (n--) {
        draw_char(x, y, buf[n], color);
        x += FONT_W + 1;
    }
}

/* ------------------------------------------------------------------
 *  Bresenham line — pointer-stepping, no per-pixel multiply
 * ------------------------------------------------------------------ */
static void draw_line(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? FB_W : -FB_W;       /* VRAM row stride */

    uint16_t *p = fb_vram + y0 * FB_W + x0;
    int err = dx - dy;

    for (;;) {
        *p = color;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; p += sx; }
        if (e2 <  dx) { err += dx; p += sy;  y0 += (sy > 0) ? 1 : -1; }
    }
}

/* ------------------------------------------------------------------
 *  Sin look-up  (first-quadrant table, 7-bit fraction, range ±127)
 *  angle: 0-255 maps to 0-2pi.   cos(a) = sin(a + 64)
 * ------------------------------------------------------------------ */
static const int8_t sin_tbl[65] = {
      0,   6,  13,  19,  25,  31,  37,  43,
     49,  55,  60,  66,  71,  76,  81,  86,
     90,  95,  99, 103, 106, 110, 113, 116,
    118, 120, 122, 124, 125, 126, 127, 127,
    127, 127, 127, 126, 125, 124, 122, 120,
    118, 116, 113, 110, 106, 103,  99,  95,
     90,  86,  81,  76,  71,  66,  60,  55,
     49,  43,  37,  31,  25,  19,  13,   6,   0
};

static int sin8(int angle)
{
    int i = angle & 0xFF;
    int neg = 0;
    if (i >= 128) { neg = 1; i -= 128; }
    if (i >  64)  i = 128 - i;
    int v = sin_tbl[i];
    return neg ? -v : v;
}


int main(int /*argc*/, char ** /*argv*/, char ** /*envp*/)
{
    int frame = 0;
    uint32_t total_ticks = 0;
    int fps = 0;

    POWER_MSTPCR0->s.TMU = 0;

    const int cx = FB_W / 2;          /*  80 */
    const int cy = FB_H / 2;          /* 132 */
    const int radius = 60;

    while (total_ticks < 0x7FFF) {
        /* ---- start TMU ---- */
        TMU_TCR_1->raw  = 0;
        TMU_TCR_1->s.TPSC = PHI_DIV_4;
        *TMU_TCOR_1 = 0xFFFFFFFFu;
        *TMU_TCNT_1 = 0xFFFFFFFFu;
        TMU_TSTR->s.STR1 = 1;
        uint32_t t_start = *TMU_TCNT_1;

        /* ---- clear to black ---- */
        fb_clear(0x0000);

        /* ---- compute spinning line endpoints ---- */
        int angle = frame * 3;                       /* ~4.2 deg/frame */
        int s = sin8(angle);
        int c = sin8(angle + 64);                    /* cos */
        int dx = (s * radius + 64) >> 7;             /* +64 for rounding */
        int dy = (c * radius + 64) >> 7;

        draw_line(cx - dx, cy - dy, cx + dx, cy + dy, 0xFFFF);

        /* ---- HUD (shown only after first FPS measurement) ---- */
        if (fps > 0) {
            fb_rect(0, 0, FB_W, FONT_H + 2, 0x0000);
            draw_text(1, 1, "FPS:", 0x07E0);
            draw_num(1 + 4 * (FONT_W + 1), 1, fps, 0x07E0);
        }

        /* ---- blit to LCD ---- */
        fb_present();

        /* ---- stop TMU ---- */
        uint32_t t_end = *TMU_TCNT_1;
        TMU_TSTR->s.STR1 = 0;
        total_ticks += t_start - t_end;

        frame++;

        /* ---- FPS: recompute every 1000 frames ---- */
        if (frame % 1000 == 0) {
            uint32_t avg = total_ticks / 1000;
            fps = avg > 0 ? (int)(TMU_TICKS_PER_SEC / avg) : 9999;
            total_ticks = 0;
        }
    }
    return 0;
}
