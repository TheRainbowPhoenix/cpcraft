/* simulator/src/simulator.cpp */
/* simulator/src/simulator.cpp
 *
 * The ClassPad LCD + keyboard simulator.
 *
 * Architecture
 * ------------
 * This file provides definitions for all the SDK function pointers and
 * globals that the engine / app calls (LCD_*, Input_*, Debug_*, vram).
 * The app is compiled against the simulator's <sdk/...> headers, which
 * declare the same names as the real SDK — so the app compiles unchanged
 * and links against us instead of libsdk.a.
 *
 * The LCD is simulated faithfully so the engine's fb_present() blit
 * routine works exactly like on hardware:
 *
 *   - 320x528 RGB565 framebuffer (matches ClassPad resolution).
 *   - LCD_SetDrawingBounds(x0, x1, y0, y1) remembers the active window.
 *   - LCD_SendCommand(COMMAND_PREPARE_FOR_DRAW_DATA) (0x2C) resets the
 *     write cursor to (x0, y0).
 *   - Writes to *lcd_data_port fill the framebuffer at the cursor and
 *     advance it left-to-right, top-to-bottom within the window.
 *
 * That's the same protocol the ILI9341-derived controller on the ClassPad
 * uses, which is what makes our 2x-upscale blit routine work the same in
 * the simulator as on hardware.
 *
 * Keyboard mapping
 * ----------------
 * Host keys map to ClassPad scancodes (see kbd_map[]). The ClassPad's
 * directional pad maps to the host arrow keys; EXE maps to Enter or
 * Space; POWER/CLEAR (which the engine uses as "quit") maps to Escape
 * or Backspace.
 *
 * Native build (Linux/macOS/Windows)
 * ----------------------------------
 * Link with SDL2. The simulator opens a 640x1056 window (2x the ClassPad
 * resolution for readability) and renders the LCD framebuffer to it via
 * SDL_TEXTUREACCESS_STREAMING.
 *
 * Web build (Emscripten)
 * ----------------------
 * Link with -sUSE_SDL=2. The simulator runs in the browser, sized by
 * the surrounding shell.html. The main loop is driven by emscripten's
 * requestAnimationFrame via the sim_frame() hook.
 *
 * Frame timing
 * ------------
 * The engine's TMU-based timing (in src/engine/tmu.h) doesn't work on
 * host because TMU registers are at fixed SH-4A addresses. To keep the
 * engine's TMU calls from crashing, we re-route them via macros to a
 * host-side high-resolution timer (see engine_tmu_host_*). The engine
 * code is unchanged — only the macros differ at compile time.
 */
#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <queue>
#include <vector>

/* Pull in the simulator's SDK stub headers (which the engine also uses). */
#include "sdk/os/lcd.h"
#include "sdk/os/input.h"
#include "sdk/os/debug.h"
#include "sdk/calc/calc.h"

/* Pull in the engine's TMU/POWER typedefs so we can define the host-side
 * storage that backs the engine's TMU_* / POWER_MSTPCR0 macros. */
#include "engine/power.h"
#include "engine/tmu.h"

/* ============================================================================
 *  ClassPad hardware constants
 * ============================================================================
 *
 * Mirrored from the real SDK's <sdk/calc/calc.h>. Must match the values
 * in simulator/include/sdk/calc/calc.h or the engine will miscompile. */
#define CP_LCD_W   320
#define CP_LCD_H   528
/* ============================================================================
 *  LCD framebuffer + protocol state
 * ============================================================================
 *
 * Two buffers:
 *   - lcd_gram:   what the LCD panel actually shows (the ILI9341's GRAM).
 *   - cp_vram_buf: the OS-style VRAM buffer (what LCD_GetPixel/SetPixel read).
 *
 * The engine treats VRAM as a scratch buffer for rendering and streams the
 * final pixels to lcd_gram via the LCD port protocol below.
 *
 * LCD protocol (matches the real ILI9341-derived controller):
 *   1. LCD_SetDrawingBounds(x0, x1, y0, y1) sets the active window.
 *   2. LCD_SendCommand(COMMAND_PREPARE_FOR_DRAW_DATA) resets the cursor.
 *   3. Each SIM_LCD_WRITE(pixel) call writes one pixel at the cursor and
 *      advances it left-to-right, top-to-bottom within the window.
 *
 * SIM_LCD_WRITE() is a macro defined in <sdk/os/lcd.h>. On hardware it
 * expands to `*lcd_data_port = (pixel)` (a real LCD write). In the
 * simulator it expands to `sim_lcd_write(pixel)` (a function call) so
 * we can intercept the write and update lcd_gram. This is the only
 * difference between the engine's hardware and simulator builds.
 */

static uint16_t  lcd_gram[CP_LCD_W * CP_LCD_H];   /* what the LCD shows */
static uint16_t  cp_vram_buf[CP_LCD_W * CP_LCD_H]; /* OS-style VRAM */

/* The OS-style `vram` pointer exposed by <sdk/calc/calc.h>. On hardware
 * this is 0x8c000000; in the simulator it points at cp_vram_buf. */
uint16_t * const vram = cp_vram_buf;

/* LCD protocol state — set by LCD_SetDrawingBounds / LCD_SendCommand. */
static unsigned int lb_x0 = 0, lb_x1 = CP_LCD_W - 1;
static unsigned int lb_y0 = 0, lb_y1 = CP_LCD_H - 1;
static unsigned int lb_cursor_x = 0;
static unsigned int lb_cursor_y = 0;
static bool         lb_drawing = false;  /* true after COMMAND_PREPARE_FOR_DRAW_DATA */

/* The data-port write handler. Called by sim_lcd_write() below for each
 * pixel the engine streams to the LCD. */
static void lcd_data_write(uint16_t pixel)
{
    if (lb_drawing && lb_cursor_x <= lb_x1 && lb_cursor_y <= lb_y1)
    {
        lcd_gram[lb_cursor_y * CP_LCD_W + lb_cursor_x] = pixel;
    }
    /* Advance cursor: left-to-right, then top-to-bottom. */
    lb_cursor_x++;
    if (lb_cursor_x > lb_x1)
    {
        lb_cursor_x = lb_x0;
        lb_cursor_y++;
        if (lb_cursor_y > lb_y1)
        {
            /* Past the end of the window — stop drawing. Real hardware
             * would just ignore the extra writes; we do the same. */
            lb_drawing = false;
        }
    }
}

/* lcd_data_port points at this dummy on the simulator side. Direct writes
 * via `*lcd_data_port = X` (if any code still does that) land here and
 * are silently ignored. Code that uses SIM_LCD_WRITE() goes through
 * sim_lcd_write() and properly updates lcd_gram. */
static volatile uint16_t lcd_data_sink = 0;
volatile uint16_t *lcd_data_port = &lcd_data_sink;

/* The function the engine's SIM_LCD_WRITE() macro calls. */
extern "C" void sim_lcd_write(uint16_t pixel)
{
    lcd_data_write(pixel);
}

/* ============================================================================
 *  SDK function implementations
 * ============================================================================ */

/* --- LCD --- */

static void sim_LCD_SetDrawingBounds(unsigned int xstart, unsigned int xend,
                                     unsigned int ystart, unsigned int yend)
{
    lb_x0 = xstart;
    lb_x1 = xend;
    lb_y0 = ystart;
    lb_y1 = yend;
}

static void sim_LCD_SendCommandU(uint16_t command)
{
    if (command == COMMAND_PREPARE_FOR_DRAW_DATA)
    {
        lb_cursor_x = lb_x0;
        lb_cursor_y = lb_y0;
        lb_drawing = true;
    }
}

static void sim_LCD_Refresh()
{
    /* The OS uses LCD_Refresh to copy VRAM to the LCD. Our engine
     * writes directly to the LCD port instead, so LCD_Refresh is a
     * no-op for us. We provide it for completeness. */
}

static uint16_t *sim_LCD_GetVRAMAddress()
{
    return cp_vram_buf;
}

static void sim_LCD_SetPixel(unsigned int x, unsigned int y, uint16_t color)
{
    if (x < CP_LCD_W && y < CP_LCD_H)
        cp_vram_buf[y * CP_LCD_W + x] = color;
}

static uint16_t sim_LCD_GetPixel(unsigned int x, unsigned int y)
{
    if (x < CP_LCD_W && y < CP_LCD_H)
        return cp_vram_buf[y * CP_LCD_W + x];
    return 0;
}

static void sim_LCD_GetSize(unsigned int *w, unsigned int *h)
{
    if (w) *w = CP_LCD_W;
    if (h) *h = CP_LCD_H;
}

static void sim_LCD_SetPixelFromPaletteU(unsigned int x, unsigned int y, uint8_t index)
{
    static const uint16_t palette[8] = {
        0x0000, 0x001F, 0x07E0, 0x07FF,
        0xF800, 0xF81F, 0xFFE0, 0xFFFF
    };
    if (index < 8) sim_LCD_SetPixel(x, y, palette[index]);
}

static void sim_LCD_VRAMBackup() {}
static void sim_LCD_VRAMRestore() {}
static void sim_LCD_ClearScreen()
{
    for (int i = 0; i < CP_LCD_W * CP_LCD_H; i++) cp_vram_buf[i] = 0xFFFF;
}

/* --- Input --- */

/* Scancode -> "currently down" map. Set by SDL event loop. */
static bool key_state[65536] = {false};

/* Touch state (mouse). */
static bool touch_down = false;
static int  touch_x = 0, touch_y = 0;

static bool sim_Input_GetKeyStateU(uint16_t *scancode)
{
    return key_state[*scancode];
}

static bool sim_Input_IsAnyKeyDown()
{
    for (int i = 0; i < 65536; i++)
        if (key_state[i]) return true;
    return false;
}

/* Simple event queue for the simulator's GetInput() implementation.
 *
 * When SDL reports a keydown, we push a KEY_PRESSED event. When SDL reports
 * a keyup, we push a KEY_RELEASED event. The engine's input_update() drains
 * this queue via GetInput().
 *
 * We don't generate KEY_HELD events — the engine's input pattern retains
 * flags between frames, so as long as we send KEY_PRESSED on press and
 * KEY_RELEASED on release, the engine sees the correct state. */
struct SimEvent {
    enum Input_EventType type;
    int  direction;   /* KEY_PRESSED, KEY_RELEASED, etc. */
    int  keycode;     /* KEYCODE_* */
};
static std::queue<SimEvent> sim_event_queue;

/* scancode_to_keycode: map a ClassPad scancode (used by the SDL handler)
 * to a KEYCODE_* value (used by the engine's input_update()).
 *
 * The scancodes and keycodes are different numbering schemes from the
 * ClassPad OS. The SDL handler stores scancodes; the engine expects
 * keycodes. This table bridges them.
 *
 * Only the keys the engine cares about are mapped; others return 0. */
static int scancode_to_keycode(uint16_t sc)
{
    switch (sc)
    {
        case ScancodeUp:         return KEYCODE_UP;
        case ScancodeDown:       return KEYCODE_DOWN;
        case ScancodeLeft:       return KEYCODE_LEFT;
        case ScancodeRight:      return KEYCODE_RIGHT;
        case ScancodeEXE:        return KEYCODE_EXE;
        case ScancodeBackspace:  return KEYCODE_BACKSPACE;
        case ScancodeShift:      return KEYCODE_SHIFT;
        case ScancodeClear:      return KEYCODE_POWER_CLEAR;
        case Scancode0:          return KEYCODE_0;
        case Scancode1:          return KEYCODE_1;
        case Scancode2:          return KEYCODE_2;
        case Scancode3:          return KEYCODE_3;
        case Scancode4:          return KEYCODE_4;
        case Scancode5:          return KEYCODE_5;
        case Scancode6:          return KEYCODE_6;
        case Scancode7:          return KEYCODE_7;
        case Scancode8:          return KEYCODE_8;
        case Scancode9:          return KEYCODE_9;
        case ScancodePlus:       return KEYCODE_PLUS;
        case ScancodeMinus:      return KEYCODE_MINUS;
        case ScancodeDot:        return KEYCODE_DOT;
        case ScancodeEXP:        return KEYCODE_EXP;
        case ScancodeDivide:     return KEYCODE_DIVIDE;
        case ScancodeTimes:      return KEYCODE_TIMES;
        default:                 return 0;
    }
}

static int sim_GetInput(struct Input_Event *event, uint32_t unknown1, uint32_t unknown2)
{
    (void)unknown1; (void)unknown2;

    /* Pop one event from the queue. If empty, return EVENT_NONE. */
    if (sim_event_queue.empty())
    {
        event->type = EVENT_NONE;
        return 0;
    }

    SimEvent se = sim_event_queue.front();
    sim_event_queue.pop();

    /* Zero the struct first (matches the CP-Raycaster pattern of
     * Mem_Memset'ing between calls). */
    event->type = (enum Input_EventType)0;
    event->zero = 0;
    event->data.key.direction = (enum Input_KeyEventType)0;
    event->data.key.keyCode   = (enum Input_Keycode)0;

    event->type = se.type;
    event->data.key.direction = (enum Input_KeyEventType)se.direction;
    event->data.key.keyCode   = (enum Input_Keycode)se.keycode;

    return 0;
}

/* --- Debug --- */

/* Tiny 4x6 font for Debug_Printf overlay. Same as engine/overlay.c's
 * font — duplicated here so the simulator doesn't depend on the engine. */
extern const uint8_t sim_font4x6[96][6];
extern const uint16_t sim_font_palette[8];

static void sim_Debug_Printf(unsigned int x, unsigned int y, bool invert,
                             int zero, const char *format, ...)
{
    (void)invert; (void)zero;
    char buf[256];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    /* Render at (x, y) in the LCD framebuffer using the 4x6 font.
     * Coordinates are in pixels. */
    int px = (int)x, py = (int)y;
    for (char *p = buf; *p; p++)
    {
        uint8_t c = (uint8_t)*p;
        if (c < 32 || c > 127) c = ' ';
        const uint8_t *g = sim_font4x6[c - 32];
        for (int row = 0; row < 6; row++)
        {
            uint8_t bits = g[row];
            for (int col = 0; col < 4; col++)
            {
                if (bits & (0x80 >> col))
                {
                    int tx = px + col, ty = py + row;
                    if (tx >= 0 && tx < CP_LCD_W && ty >= 0 && ty < CP_LCD_H)
                        lcd_gram[ty * CP_LCD_W + tx] = 0xFFFF;
                }
            }
        }
        px += 5;
    }
}

static bool sim_Debug_PrintString(const char *string, bool invert)
{
    (void)string; (void)invert;
    return true;
}

static int sim_Debug_SetCursorPosition(unsigned int x, unsigned int y)
{
    (void)x; (void)y;
    return 0;
}

static void sim_Debug_GetCursorPosition(unsigned int *x, unsigned int *y)
{
    if (x) *x = 0;
    if (y) *y = 0;
}

static void sim_Debug_PrintNumberHex_Byte(uint8_t v, unsigned int x, unsigned int y)
{
    char b[8]; snprintf(b, sizeof(b), "%02X", v);
    sim_Debug_Printf(x, y, false, 0, "%s", b);
}
static void sim_Debug_PrintNumberHex_Word(uint16_t v, unsigned int x, unsigned int y)
{
    char b[8]; snprintf(b, sizeof(b), "%04X", v);
    sim_Debug_Printf(x, y, false, 0, "%s", b);
}
static void sim_Debug_PrintNumberHex_Dword(uint32_t v, unsigned int x, unsigned int y)
{
    char b[12]; snprintf(b, sizeof(b), "%08lX", (unsigned long)v);
    sim_Debug_Printf(x, y, false, 0, "%s", b);
}
static void sim_Debug_PrintNumberHex_Nibble(uint8_t v, unsigned int x, unsigned int y)
{
    char b[4]; snprintf(b, sizeof(b), "%X", v & 0xF);
    sim_Debug_Printf(x, y, false, 0, "%s", b);
}

/* Pending key for Debug_WaitKey(). */
static int sim_pending_waitkey = 0;

/* Forward declarations — sim_pump_events() and sim_present() are defined
 * later in the file but Debug_WaitKey needs them now. */
extern "C" void sim_pump_events();
extern "C" void sim_present();

static int sim_Debug_WaitKey()
{
    /* Pump SDL events until a key is pressed. */
    while (sim_pending_waitkey == 0)
    {
        sim_pump_events();
        SDL_Delay(10);
    }
    int k = sim_pending_waitkey;
    sim_pending_waitkey = 0;
    return k;
}

/* --- calc.h stubs --- */

void calcInit(void) {}
void calcExit(void) {}
void line(unsigned int, unsigned int, unsigned int, unsigned int, uint16_t) {}
void vline(unsigned int, unsigned int, unsigned int, uint16_t) {}
void triangle(unsigned int, unsigned int, unsigned int, unsigned int,
              unsigned int, unsigned int, uint16_t, uint16_t) {}
void fillScreen(uint16_t) {}
void getKey(uint32_t *k1, uint32_t *k2) { if (k1) *k1 = 0; if (k2) *k2 = 0; }

/* ============================================================================
 *  Bind the function pointers
 * ============================================================================
 *
 * These have to be assigned at runtime because the SDK declares them as
 * function pointers (the OS fills them in at startup on hardware). */
void (*LCD_SetDrawingBounds)(unsigned int, unsigned int, unsigned int, unsigned int) = sim_LCD_SetDrawingBounds;
void (*LCD_SendCommandU)(uint16_t) = sim_LCD_SendCommandU;
void (*LCD_Refresh)() = sim_LCD_Refresh;
uint16_t *(*_FP_LCD_GetVRAMAddress)() = sim_LCD_GetVRAMAddress;
void (*LCD_SetPixel)(unsigned int, unsigned int, uint16_t) = sim_LCD_SetPixel;
void (*LCD_SetPixelFromPaletteU)(unsigned int, unsigned int, uint8_t) = sim_LCD_SetPixelFromPaletteU;
void (*LCD_VRAMBackup)() = sim_LCD_VRAMBackup;
void (*LCD_VRAMRestore)() = sim_LCD_VRAMRestore;
void (*LCD_ClearScreen)() = sim_LCD_ClearScreen;
uint16_t (*LCD_GetPixel)(unsigned int, unsigned int) = sim_LCD_GetPixel;
void (*LCD_GetSize)(unsigned int *, unsigned int *) = sim_LCD_GetSize;

bool (*Input_GetKeyStateU)(uint16_t *) = sim_Input_GetKeyStateU;
bool (*Input_IsAnyKeyDown)() = sim_Input_IsAnyKeyDown;
int (*GetInput)(struct Input_Event *, uint32_t, uint32_t) = sim_GetInput;

void (*Debug_Printf)(unsigned int, unsigned int, bool, int, const char *, ...) = sim_Debug_Printf;
bool (*Debug_PrintString)(const char *, bool) = sim_Debug_PrintString;
int  (*Debug_SetCursorPosition)(unsigned int, unsigned int) = sim_Debug_SetCursorPosition;
void (*Debug_GetCursorPosition)(unsigned int *, unsigned int *) = sim_Debug_GetCursorPosition;
void (*Debug_PrintNumberHex_Byte)(uint8_t, unsigned int, unsigned int) = sim_Debug_PrintNumberHex_Byte;
void (*Debug_PrintNumberHex_Word)(uint16_t, unsigned int, unsigned int) = sim_Debug_PrintNumberHex_Word;
void (*Debug_PrintNumberHex_Dword)(uint32_t, unsigned int, unsigned int) = sim_Debug_PrintNumberHex_Dword;
void (*Debug_PrintNumberHex_Nibble)(uint8_t, unsigned int, unsigned int) = sim_Debug_PrintNumberHex_Nibble;
int (*Debug_WaitKey)() = sim_Debug_WaitKey;
uint16_t *DEBUG_FONTBASE = nullptr;

/* ============================================================================
 *  4x6 font for Debug_Printf
 * ============================================================================
 *
 * Same as engine/overlay.c's font. Duplicated here so the simulator
 * doesn't link against the engine. */
const uint8_t sim_font4x6[96][6] = {
    {0,0,0,0,0,0},
    {0x10,0x10,0x10,0x00,0x10,0x00},
    {0x50,0x50,0x00,0x00,0x00,0x00},
    {0x50,0xF0,0x50,0xF0,0x50,0x00},
    {0x40,0xE0,0xC0,0xE0,0x20,0x00},
    {0x60,0x10,0x20,0x40,0xC0,0x00},
    {0x40,0xA0,0x40,0xA0,0x60,0x00},
    {0x10,0x10,0x00,0x00,0x00,0x00},
    {0x20,0x40,0x40,0x40,0x20,0x00},
    {0x80,0x40,0x40,0x40,0x80,0x00},
    {0x00,0xA0,0x40,0xA0,0x00,0x00},
    {0x00,0x40,0xE0,0x40,0x00,0x00},
    {0x00,0x00,0x00,0x40,0x20,0x00},
    {0x00,0x00,0xE0,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x40,0x00,0x00},
    {0x20,0x20,0x40,0x80,0x80,0x00},
    {0x40,0xA0,0xA0,0xA0,0x40,0x00},
    {0x40,0xC0,0x40,0x40,0xE0,0x00},
    {0xC0,0x20,0x40,0x80,0xE0,0x00},
    {0xE0,0x20,0x40,0x20,0xE0,0x00},
    {0x20,0x60,0xA0,0xE0,0x20,0x00},
    {0xE0,0x80,0xC0,0x20,0xC0,0x00},
    {0x60,0x80,0xC0,0xA0,0x40,0x00},
    {0xE0,0x20,0x40,0x80,0x80,0x00},
    {0x40,0xA0,0x40,0xA0,0x40,0x00},
    {0x40,0xA0,0x60,0x20,0xC0,0x00},
    {0x00,0x40,0x00,0x40,0x00,0x00},
    {0x00,0x40,0x00,0x40,0x20,0x00},
    {0x20,0x40,0x80,0x40,0x20,0x00},
    {0x00,0xE0,0x00,0xE0,0x00,0x00},
    {0x80,0x40,0x20,0x40,0x80,0x00},
    {0xE0,0x20,0x40,0x00,0x40,0x00},
    {0x40,0xE0,0xA0,0x80,0x60,0x00},
    {0x40,0xA0,0xE0,0xA0,0xA0,0x00},
    {0xC0,0xA0,0xC0,0xA0,0xC0,0x00},
    {0x60,0x80,0x80,0x80,0x60,0x00},
    {0xC0,0xA0,0xA0,0xA0,0xC0,0x00},
    {0xE0,0x80,0xC0,0x80,0xE0,0x00},
    {0xE0,0x80,0xC0,0x80,0x80,0x00},
    {0x60,0x80,0xA0,0xA0,0x60,0x00},
    {0xA0,0xA0,0xE0,0xA0,0xA0,0x00},
    {0xE0,0x40,0x40,0x40,0xE0,0x00},
    {0x20,0x20,0x20,0xA0,0x40,0x00},
    {0xA0,0xA0,0xC0,0xA0,0xA0,0x00},
    {0x80,0x80,0x80,0x80,0xE0,0x00},
    {0xA0,0xE0,0xE0,0xA0,0xA0,0x00},
    {0xA0,0xE0,0xE0,0xE0,0xA0,0x00},
    {0x40,0xA0,0xA0,0xA0,0x40,0x00},
    {0xC0,0xA0,0xC0,0x80,0x80,0x00},
    {0x40,0xA0,0xA0,0xE0,0x60,0x00},
    {0xC0,0xA0,0xC0,0xA0,0xA0,0x00},
    {0x60,0x80,0x40,0x20,0xC0,0x00},
    {0xE0,0x40,0x40,0x40,0x40,0x00},
    {0xA0,0xA0,0xA0,0xA0,0x40,0x00},
    {0xA0,0xA0,0xA0,0xA0,0x40,0x00},
    {0xA0,0xA0,0xE0,0xE0,0xA0,0x00},
    {0xA0,0xA0,0x40,0xA0,0xA0,0x00},
    {0xA0,0xA0,0x40,0x40,0x40,0x00},
    {0xE0,0x20,0x40,0x80,0xE0,0x00},
    {0xC0,0x80,0x80,0x80,0xC0,0x00},
    {0x80,0x80,0x40,0x20,0x20,0x00},
    {0xC0,0x40,0x40,0x40,0xC0,0x00},
    {0x40,0xA0,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0xE0,0x00},
    {0x80,0x40,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x60,0x20,0x60,0x00},
    {0x80,0x80,0xC0,0xA0,0xC0,0x00},
    {0x00,0x00,0x60,0x80,0x60,0x00},
    {0x20,0x20,0x60,0xA0,0x60,0x00},
    {0x00,0x00,0x40,0xA0,0x60,0x00},
    {0x20,0x40,0xE0,0x40,0x40,0x00},
    {0x00,0x60,0xA0,0x60,0x20,0xC0},
    {0x80,0x80,0xC0,0xA0,0xA0,0x00},
    {0x40,0x00,0x40,0x40,0x40,0x00},
    {0x20,0x00,0x20,0x20,0x20,0xC0},
    {0x80,0x80,0xA0,0xC0,0xA0,0x00},
    {0x40,0x40,0x40,0x40,0x40,0x00},
    {0x00,0x00,0xA0,0xE0,0xA0,0x00},
    {0x00,0x00,0xC0,0xA0,0xA0,0x00},
    {0x00,0x00,0x40,0xA0,0x40,0x00},
    {0x00,0xC0,0xA0,0xC0,0x80,0x00},
    {0x00,0x60,0xA0,0x60,0x20,0x00},
    {0x00,0x00,0xC0,0xA0,0x80,0x00},
    {0x00,0x60,0xC0,0x20,0xC0,0x00},
    {0x40,0x40,0xE0,0x40,0x20,0x00},
    {0x00,0x00,0xA0,0xA0,0x60,0x00},
    {0x00,0x00,0xA0,0xA0,0x40,0x00},
    {0x00,0x00,0xA0,0xE0,0xA0,0x00},
    {0x00,0x00,0xA0,0x40,0xA0,0x00},
    {0x00,0xA0,0xA0,0x60,0x20,0xC0},
    {0x00,0xE0,0x20,0x40,0xE0,0x00},
    {0x20,0x40,0x80,0x40,0x20,0x00},
    {0x40,0x40,0x40,0x40,0x40,0x00},
    {0x80,0x40,0x20,0x40,0x80,0x00},
    {0x00,0x60,0xC0,0x00,0x00,0x00},
    {0xE0,0xE0,0xE0,0xE0,0xE0,0x00},
};
const uint16_t sim_font_palette[8] = {
    0x0000, 0x001F, 0x07E0, 0x07FF,
    0xF800, 0xF81F, 0xFFE0, 0xFFFF
};

/* ============================================================================
 *  TMU + POWER_MSTPCR0 host-side storage
 * ============================================================================
 *
 * These back the macros in engine/tmu.h and engine/power.h when building
 * for the simulator (i.e. not __sh__). The engine writes to them as if
 * they were the real MMIO registers; we accept the writes silently.
 *
 * sim_tmu_tcnt_1_storage is the one that matters for frame timing — the
 * engine reads it via *TMU_TCNT_1 to compute "ticks since last frame".
 * We update it from sim_present() based on SDL_GetTicks(), scaled to be
 * in the same ballpark as the real 14.5 MHz TMU clock. The exact ratio
 * doesn't matter — only the relative deltas do, and the engine computes
 * those by subtracting two reads of the same counter. */
power_mstpcr0 sim_power_mstpcr0 = {};
tmu_tstr sim_tmu_tstr = {};
tmu_tcr  sim_tmu_tcr_0 = {}, sim_tmu_tcr_1 = {}, sim_tmu_tcr_2 = {};
uint32_t sim_tmu_tcor_0 = 0, sim_tmu_tcor_1 = 0, sim_tmu_tcor_2 = 0;
uint32_t sim_tmu_tcnt_0 = 0, sim_tmu_tcnt_1_storage = 0xFFFFFFFFu, sim_tmu_tcnt_2 = 0;

/* Last SDL_GetTicks() value we used to update sim_tmu_tcnt_1_storage.
 * We use this to compute a delta on each sim_present() call. */
static uint32_t sim_last_sdl_ticks = 0;

/* Update sim_tmu_tcnt_1_storage to reflect the elapsed host time.
 * Called from sim_present() each frame.
 *
 * The engine writes `*TMU_TCNT_1 = 0xFFFFFFFFu` at startup (to reset
 * the counter) and reads `*TMU_TCNT_1` later to get a countdown. The
 * delta (start - end) is the elapsed ticks.
 *
 * On host, we compute the equivalent: scale SDL_GetTicks() (ms) by
 * TMU_TICKS_PER_USEC * 1000 (ticks/sec) -> ticks/ms = 8000. So
 * 1 ms of host time = 8000 TMU ticks. A 60 FPS frame (16.67 ms) is
 * then ~133 333 ticks, which is in the same range as the hardware's
 * ~140 000 ticks / frame the benchmark measured. */
static void sim_tmu_update()
{
    uint32_t now = SDL_GetTicks();
    if (sim_last_sdl_ticks == 0) sim_last_sdl_ticks = now;
    uint32_t elapsed_ms = now - sim_last_sdl_ticks;
    sim_last_sdl_ticks = now;

    /* Convert ms -> TMU ticks. */
    uint32_t elapsed_ticks = elapsed_ms * (TMU_TICKS_PER_USEC * 1000);

    /* TMU counts DOWN. If we'd underflow, clamp to 0. */
    if (elapsed_ticks > sim_tmu_tcnt_1_storage)
        sim_tmu_tcnt_1_storage = 0;
    else
        sim_tmu_tcnt_1_storage -= elapsed_ticks;
}

/* ============================================================================
 *  SDL2 display + keyboard
 * ============================================================================
 *
 * The simulator opens a window scaled 2x from the ClassPad's native
 * 320x528 to 640x1056 for readability. */

static SDL_Window   *sim_window   = nullptr;
static SDL_Renderer *sim_renderer = nullptr;
static SDL_Texture  *sim_texture  = nullptr;
static bool          sim_should_quit = false;
static int           sim_window_scale = 1;

/* Host-key -> ClassPad-scancode mapping.
 * Keys not in this map are ignored. */
struct KeyMap { SDL_Scancode host; uint16_t cp; };
static const KeyMap kbd_map[] = {
    { SDL_SCANCODE_UP,        (uint16_t)ScancodeUp },
    { SDL_SCANCODE_DOWN,      (uint16_t)ScancodeDown },
    { SDL_SCANCODE_LEFT,      (uint16_t)ScancodeLeft },
    { SDL_SCANCODE_RIGHT,     (uint16_t)ScancodeRight },
    { SDL_SCANCODE_RETURN,    (uint16_t)ScancodeEXE },
    { SDL_SCANCODE_SPACE,     (uint16_t)ScancodeEXE },     /* alt for A */
    { SDL_SCANCODE_BACKSPACE, (uint16_t)ScancodeBackspace },
    { SDL_SCANCODE_ESCAPE,    (uint16_t)ScancodeClear },   /* MENU/quit */
    { SDL_SCANCODE_0,         (uint16_t)Scancode0 },
    { SDL_SCANCODE_1,         (uint16_t)Scancode1 },
    { SDL_SCANCODE_2,         (uint16_t)Scancode2 },
    { SDL_SCANCODE_3,         (uint16_t)Scancode3 },
    { SDL_SCANCODE_4,         (uint16_t)Scancode4 },
    { SDL_SCANCODE_5,         (uint16_t)Scancode5 },
    { SDL_SCANCODE_6,         (uint16_t)Scancode6 },
    { SDL_SCANCODE_7,         (uint16_t)Scancode7 },
    { SDL_SCANCODE_8,         (uint16_t)Scancode8 },
    { SDL_SCANCODE_9,         (uint16_t)Scancode9 },
    { SDL_SCANCODE_MINUS,     (uint16_t)ScancodeMinus },
    { SDL_SCANCODE_EQUALS,    (uint16_t)ScancodeEquals },
    { SDL_SCANCODE_LEFTBRACKET, (uint16_t)ScancodeOpenParenthesis },
    { SDL_SCANCODE_RIGHTBRACKET,(uint16_t)ScancodeCloseParenthesis },
    { SDL_SCANCODE_COMMA,     (uint16_t)ScancodeComma },
    { SDL_SCANCODE_PERIOD,    (uint16_t)ScancodeDot },
    { SDL_SCANCODE_SLASH,     (uint16_t)ScancodeDivide },
    { SDL_SCANCODE_A,         (uint16_t)ScancodeNegative },
    { SDL_SCANCODE_X,         (uint16_t)ScancodeX },
    { SDL_SCANCODE_Y,         (uint16_t)ScancodeY },
    { SDL_SCANCODE_Z,         (uint16_t)ScancodeZ },
    { SDL_SCANCODE_LSHIFT,    (uint16_t)ScancodeShift },
    { SDL_SCANCODE_RSHIFT,    (uint16_t)ScancodeShift },
    { SDL_SCANCODE_TAB,       (uint16_t)ScancodeKeyboard },
    { SDL_SCANCODE_GRAVE,     (uint16_t)ScancodePower },
};

static uint16_t scancode_for_host(SDL_Scancode s)
{
    for (const auto &m : kbd_map)
        if (m.host == s) return m.cp;
    return 0;
}

/* Pump SDL events. Called from sim_pump_events() and from the engine's
 * vblank hook. */
extern "C" void sim_pump_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                sim_should_quit = true;
                exit(0);
                break;
            case SDL_KEYDOWN:
            {
                uint16_t cp = scancode_for_host(e.key.keysym.scancode);
                if (cp) {
                    key_state[cp] = true;
                    sim_pending_waitkey = cp;
                    /* Also push a KEY_PRESSED event into the GetInput queue
                     * so the engine's input_update() sees it. */
                    int kc = scancode_to_keycode(cp);
                    if (kc)
                        sim_event_queue.push({EVENT_KEY, KEY_PRESSED, kc});
                }
                break;
            }
            case SDL_KEYUP:
            {
                uint16_t cp = scancode_for_host(e.key.keysym.scancode);
                if (cp) {
                    key_state[cp] = false;
                    /* Push a KEY_RELEASED event. */
                    int kc = scancode_to_keycode(cp);
                    if (kc)
                        sim_event_queue.push({EVENT_KEY, KEY_RELEASED, kc});
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                touch_down = true;
                touch_x = e.button.x / sim_window_scale;
                touch_y = e.button.y / sim_window_scale;
                break;
            case SDL_MOUSEBUTTONUP:
                touch_down = false;
                break;
            case SDL_MOUSEMOTION:
                if (touch_down) {
                    touch_x = e.motion.x / sim_window_scale;
                    touch_y = e.motion.y / sim_window_scale;
                }
                break;
        }
    }
}

/* Present the LCD framebuffer to the SDL window. Called after each engine
 * frame. */
extern "C" void sim_present()
{
    if (!sim_texture) return;
    sim_tmu_update();   /* advance TMU channel 1 by the elapsed host time */
    SDL_UpdateTexture(sim_texture, nullptr, lcd_gram, CP_LCD_W * sizeof(uint16_t));
    SDL_RenderClear(sim_renderer);
    SDL_RenderCopy(sim_renderer, sim_texture, nullptr, nullptr);
    SDL_RenderPresent(sim_renderer);
    sim_pump_events();
}

/* Initialize the simulator. Must be called before the engine's main(). */
extern "C" void simulator_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    char title[128] = "cpcraft-port simulator";
    sim_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        CP_LCD_W * sim_window_scale, CP_LCD_H * sim_window_scale,
        SDL_WINDOW_SHOWN);

    sim_renderer = SDL_CreateRenderer(
        sim_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    sim_texture = SDL_CreateTexture(
        sim_renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        CP_LCD_W, CP_LCD_H);

    /* Clear the LCD to white. */
    for (int i = 0; i < CP_LCD_W * CP_LCD_H; i++) lcd_gram[i] = 0xFFFF;
}

/* For emscripten: a single frame hook that the main loop calls. */
extern "C" int sim_frame()
{
    sim_present();
    return sim_should_quit ? 0 : 1;
}
