/* test/native_test.c */
/*
 * Native test harness for cpcraft-port engine.
 *
 * Stubs out the SDK function pointers and runs a few basic tests to verify
 * the engine's logic works end-to-end (modulo the LCD/Input calls being
 * no-ops). This is for catching logic bugs before flashing to hardware.
 *
 * Build (via `make test` in the project root):
 *   gcc -std=c23 -O2 -I src -I test/sdk-stub \
 *       -D'__attribute__(x)=' -D'__asm__(x)=' \
 *       -include stdint.h -include stdbool.h -include stddef.h \
 *       src/engine/*.c src/demo/*.c test/native_test.c -o /tmp/... -lm
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sdk/os/input.h>
#include <sdk/os/lcd.h>
#include <sdk/os/debug.h>
#include <sdk/calc/calc.h>
#include "engine/engine.h"

/* ============================================================================
 *  SDK function pointer stubs
 * ============================================================================
 *
 * The engine calls SDK functions via function pointers (LCD_SetDrawingBounds,
 * GetInput, Debug_Printf, etc.). The real SDK fills these in at startup; we
 * provide no-op stubs here so the engine links. */

static void stub_set_bounds(unsigned x0, unsigned x1, unsigned y0, unsigned y1) {
    (void)x0; (void)x1; (void)y0; (void)y1;
}
static void stub_send_cmd(uint16_t c) { (void)c; }
static void stub_refresh(void) {}
static int  stub_waitkey(void) { return 0; }
static void stub_printf(unsigned x, unsigned y, bool i, int z, const char *f, ...) {
    (void)x; (void)y; (void)i; (void)z; (void)f;
}
static bool stub_getkeystate(uint16_t *sc) { (void)sc; return false; }
static bool stub_isanykeydown(void) { return false; }
static int  stub_getinput(struct Input_Event *ev, uint32_t a, uint32_t b) {
    (void)a; (void)b;
    ev->type = EVENT_NONE;
    return 0;
}
static uint16_t stub_vram_buf[320 * 528];  /* real buffer for the test */
static uint16_t *stub_getvram(void) { return stub_vram_buf; }
static void stub_setpixel(unsigned x, unsigned y, uint16_t c) { (void)x; (void)y; (void)c; }
static uint16_t stub_getpixel(unsigned x, unsigned y) { (void)x; (void)y; return 0; }
static void stub_getsize(unsigned *w, unsigned *h) { if(w) *w = 320; if(h) *h = 528; }
static void stub_setpixelpalette(unsigned x, unsigned y, uint8_t i) { (void)x; (void)y; (void)i; }
static void stub_vrambackup(void) {}
static void stub_vramrestore(void) {}
static void stub_clearscreen(void) {}
static bool stub_printstring(const char *s, bool i) { (void)s; (void)i; return true; }
static int  stub_setcursor(unsigned x, unsigned y) { (void)x; (void)y; return 0; }
static void stub_getcursor(unsigned *x, unsigned *y) { if(x) *x=0; if(y) *y=0; }
static void stub_hexbyte(uint8_t v, unsigned x, unsigned y) { (void)v; (void)x; (void)y; }
static void stub_hexword(uint16_t v, unsigned x, unsigned y) { (void)v; (void)x; (void)y; }
static void stub_hexdword(uint32_t v, unsigned x, unsigned y) { (void)v; (void)x; (void)y; }
static void stub_hexnibble(uint8_t v, unsigned x, unsigned y) { (void)v; (void)x; (void)y; }

/* Bind the function pointers. */
void (*LCD_SetDrawingBounds)(unsigned, unsigned, unsigned, unsigned) = stub_set_bounds;
void (*LCD_SendCommandU)(uint16_t) = stub_send_cmd;
void (*LCD_Refresh)() = stub_refresh;
volatile uint16_t *lcd_data_port = 0;
uint16_t *(*_FP_LCD_GetVRAMAddress)() = stub_getvram;
void (*LCD_SetPixel)(unsigned, unsigned, uint16_t) = stub_setpixel;
uint16_t (*LCD_GetPixel)(unsigned, unsigned) = stub_getpixel;
void (*LCD_GetSize)(unsigned *, unsigned *) = stub_getsize;
void (*LCD_SetPixelFromPaletteU)(unsigned, unsigned, uint8_t) = stub_setpixelpalette;
void (*LCD_VRAMBackup)() = stub_vrambackup;
void (*LCD_VRAMRestore)() = stub_vramrestore;
void (*LCD_ClearScreen)() = stub_clearscreen;

bool (*Input_GetKeyStateU)(uint16_t *) = stub_getkeystate;
bool (*Input_IsAnyKeyDown)() = stub_isanykeydown;
int (*GetInput)(struct Input_Event *, uint32_t, uint32_t) = stub_getinput;

int (*Debug_WaitKey)() = stub_waitkey;
void (*Debug_Printf)(unsigned, unsigned, bool, int, const char *, ...) = stub_printf;
bool (*Debug_PrintString)(const char *, bool) = stub_printstring;
int (*Debug_SetCursorPosition)(unsigned, unsigned) = stub_setcursor;
void (*Debug_GetCursorPosition)(unsigned *, unsigned *) = stub_getcursor;
void (*Debug_PrintNumberHex_Byte)(uint8_t, unsigned, unsigned) = stub_hexbyte;
void (*Debug_PrintNumberHex_Word)(uint16_t, unsigned, unsigned) = stub_hexword;
void (*Debug_PrintNumberHex_Dword)(uint32_t, unsigned, unsigned) = stub_hexdword;
void (*Debug_PrintNumberHex_Nibble)(uint8_t, unsigned, unsigned) = stub_hexnibble;
uint16_t *DEBUG_FONTBASE = 0;

/* calc.h stubs. */
void calcInit(void) {}
void calcExit(void) {}
void line(unsigned, unsigned, unsigned, unsigned, uint16_t) {}
void vline(unsigned, unsigned, unsigned, uint16_t) {}
void triangle(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, uint16_t, uint16_t) {}
void fillScreen(uint16_t) {}
void getKey(uint32_t *k1, uint32_t *k2) { if(k1) *k1=0; if(k2) *k2=0; }

/* Simulator-only symbols the engine references when not on __sh__.
 * The engine's framebuffer.c calls sim_present() after each frame and
 * sim_lcd_write() via the SIM_LCD_WRITE macro. We provide no-op stubs. */
void sim_present(void) {}
void sim_lcd_write(uint16_t pixel) { (void)pixel; }

power_mstpcr0 sim_power_mstpcr0;
tmu_tstr sim_tmu_tstr;
tmu_tcr  sim_tmu_tcr_0, sim_tmu_tcr_1, sim_tmu_tcr_2;
uint32_t sim_tmu_tcor_0 = 0, sim_tmu_tcor_1 = 0, sim_tmu_tcor_2 = 0;
uint32_t sim_tmu_tcnt_0 = 0, sim_tmu_tcnt_1_storage = 0xFFFFFFFFu, sim_tmu_tcnt_2 = 0;

int main(void)
{
    /* Force unbuffered stdout so we see prints even if the test crashes. */
    setvbuf(stdout, 0, _IONBF, 0);

    /* fb_init() will set fb_vram to LCD_GetVRAMAddress(), which our stub
     * returns as stub_vram_buf (a real 320x528 buffer). */
    engine_init();

    printf("== cpcraft-port native test ==\n");
    printf("after engine_init (fb_vram=%p)\n", (void *)fb_vram); fflush(stdout);

    /* Test 1: framebuffer basic ops. */
    fb_clear(0x1234);
    uint16_t p = fb_get_pixel(10, 10);
    printf("[1] fb_clear(0x1234) -> fb_get_pixel(10,10) = 0x%04X  %s\n",
           p, p == 0x1234 ? "PASS" : "FAIL");

    /* Test 2: fb_pixel. */
    fb_pixel(5, 5, 0xABCD);
    p = fb_get_pixel(5, 5);
    printf("[2] fb_pixel(5,5,0xABCD) -> fb_get_pixel(5,5) = 0x%04X  %s\n",
           p, p == 0xABCD ? "PASS" : "FAIL");

    /* Test 3: fb_hline. */
    fb_clear(0);
    fb_hline(0, FB_W - 1, 50, 0xBEEF);
    bool ok = true;
    for (int x = 0; x < FB_W; x++) if (fb_get_pixel(x, 50) != 0xBEEF) { ok = false; break; }
    printf("[3] fb_hline(0..%d, 50, 0xBEEF)  %s\n", FB_W - 1, ok ? "PASS" : "FAIL");

    /* Test 4: fb_vline. */
    fb_clear(0);
    fb_vline(80, 0, FB_H - 1, 0xCAFE);
    ok = true;
    for (int y = 0; y < FB_H; y++) if (fb_get_pixel(80, y) != 0xCAFE) { ok = false; break; }
    printf("[4] fb_vline(80, 0..%d, 0xCAFE)  %s\n", FB_H - 1, ok ? "PASS" : "FAIL");

    /* Test 5: fb_rect. */
    fb_clear(0);
    fb_rect(10, 10, 20, 20, 0xDEAD);
    ok = true;
    for (int y = 10; y < 30; y++) for (int x = 10; x < 30; x++)
        if (fb_get_pixel(x, y) != 0xDEAD) { ok = false; goto done5; }
    done5:
    printf("[5] fb_rect(10,10,20,20,0xDEAD)  %s\n", ok ? "PASS" : "FAIL");

    /* Test 6: textured rect. */
    fb_clear(0);
    fb_textured_rect(0, 0, 16, 16, tex_checker, 0, 0);
    p = fb_get_pixel(0, 0);
    uint16_t tex00 = tex_checker[0][0];
    printf("[6] fb_textured_rect(0,0,16,16,...) -> fb_get_pixel(0,0) = 0x%04X, tex[0][0]=0x%04X  %s\n",
           p, tex00, p == tex00 ? "PASS" : "FAIL");

    /* Test 7: overlay_char. */
    fb_clear(0);
    overlay_char(0, 0, 'A', 0xFFFF);
    ok = false;
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (fb_get_pixel(x, y) == 0xFFFF) { ok = true; }
    printf("[7] overlay_char(0,0,'A',0xFFFF)  %s\n", ok ? "PASS" : "FAIL");

    /* Test 8: math_lut sin lookups. */
    uint8_t s0   = fast_sin8(0);
    uint8_t s64  = fast_sin8(64);
    uint8_t s128 = fast_sin8(128);
    bool t8 = (s0 == 127 || s0 == 128) && s64 == 255 && (s128 == 127 || s128 == 128);
    printf("[8] sin8(0)=%u  sin8(64)=%u  sin8(128)=%u  %s\n",
           s0, s64, s128, t8 ? "PASS" : "FAIL");

    /* Test 9: input_update runs without crashing (GetInput returns EVENT_NONE). */
    input_update();
    ok = (input.down[EK_UP] == 0);
    printf("[9] input_update() with no events -> down[UP]=0  %s\n", ok ? "PASS" : "FAIL");

    /* Test 10: input_exit_requested() is false when no keys are down. */
    ok = !input_exit_requested();
    printf("[10] input_exit_requested() with no keys -> false  %s\n", ok ? "PASS" : "FAIL");

    printf("\nAll tests complete.\n");
    return 0;
}
