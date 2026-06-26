/* simulator/include/sdk/os/lcd.h */
/* simulator/include/sdk/os/lcd.h
 *
 * Drop-in replacement for the HHK3 SDK's <sdk/os/lcd.h>.
 *
 * Same public API as the real header, but `lcd_data_port` is a real
 * modifiable pointer (the real SDK has it as `volatile uint16_t *const`
 * because the port never moves on hardware). The simulator's port points
 * at the LCD protocol handler — see simulator.cpp's `lcd_data_write()`.
 *
 * LCD protocol simulation:
 *   1. App calls LCD_SetDrawingBounds(x0, x1, y0, y1) -> remembers bounds.
 *   2. App calls LCD_SendCommand(COMMAND_PREPARE_FOR_DRAW_DATA) (0x2C) ->
 *      resets the write cursor to (x0, y0).
 *   3. App writes 16-bit pixels to *lcd_data_port in order ->
 *      each write fills the LCD framebuffer at the cursor and advances it
 *      left-to-right, top-to-bottom, wrapping at x1+1 to (x0, y+1).
 *
 * That's exactly what the ClassPad's ILI9341-derived controller does, and
 * it's what our engine's fb_present() relies on.
 */
#pragma once

#include <sdk/compiler.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LCD_Palette {
    PALETTE_BLACK = 0,
    PALETTE_BLUE  = 1,
    PALETTE_GREEN = 2,
    PALETTE_CYAN  = 3,
    PALETTE_RED   = 4,
    PALETTE_MAGENTA = 5,
    PALETTE_YELLOW = 6,
    PALETTE_WHITE = 7
};

enum LCD_Command {
    COMMAND_SET_X_WINDOW = 0x2A,
    COMMAND_SET_Y_WINDOW = 0x2B,
    COMMAND_PREPARE_FOR_DRAW_DATA = 0x2C,
    COMMAND_READ_DRAW_DATA = 0x2E
};

/* The LCD data port. Writes go to the simulator's LCD framebuffer via the
 * protocol above. Non-const so the simulator can redirect it during init. */
extern volatile uint16_t *lcd_data_port;

#define RGB_TO_RGB565(r, g, b) ( \
    ((r & 0x1F) << 11) | \
    ((g & 0x3F) << 5)  | \
    (b & 0x1F) \
)

#define RGB565_TO_R(rgb565) ((rgb565 >> 11) & 0x1F)
#define RGB565_TO_G(rgb565) ((rgb565 >> 5)  & 0x3F)
#define RGB565_TO_B(rgb565) (rgb565 & 0x1F)

extern void (*LCD_ClearScreen)();
extern uint16_t (*LCD_GetPixel)(unsigned int x, unsigned int y);
extern void (*LCD_GetSize)(unsigned int *width, unsigned int *height);
extern uint16_t *(*_FP_LCD_GetVRAMAddress)();

/* Returns a pointer to the simulated VRAM (same layout as the real one:
 * 320 * 528 uint16_t values, row-major). The simulator allocates this
 * as a real heap buffer; on hardware it's at 0x8c000000. */
static SDK_ALWAYS_INLINE uint16_t *LCD_GetVRAMAddress() {
    return _FP_LCD_GetVRAMAddress();
}

extern void (*LCD_Refresh)();
extern void (*LCD_SetPixel)(unsigned int x, unsigned int y, uint16_t color);
extern void (*LCD_SetPixelFromPaletteU)(unsigned int x, unsigned int y, uint8_t index);
static inline void LCD_SetPixelFromPalette(unsigned int x, unsigned int y, enum LCD_Palette color) {
    LCD_SetPixelFromPaletteU(x, y, (uint8_t)color);
}

extern void (*LCD_VRAMBackup)();
extern void (*LCD_VRAMRestore)();
extern void (*LCD_SendCommandU)(uint16_t command);
static inline void LCD_SendCommand(enum LCD_Command command) {
    LCD_SendCommandU((uint16_t)command);
}

extern void (*LCD_SetDrawingBounds)(unsigned int xstart, unsigned int xend,
                                    unsigned int ystart, unsigned int yend);

/* ============================================================================
 *  SIM_LCD_WRITE — the only difference between hw and simulator builds
 * ============================================================================
 *
 * The engine's fb_present() blit routine streams pixels to the LCD by
 * writing to *lcd_data_port in a tight loop. On hardware this is a real
 * volatile store to memory-mapped I/O; the LCD controller picks it up.
 *
 * In the simulator, we can't intercept a plain volatile store (C doesn't
 * allow that). So the engine uses the SIM_LCD_WRITE(pixel) macro instead
 * of `*lcd_data_port = pixel`:
 *
 *   - On hardware (real SDK): SIM_LCD_WRITE(p) expands to `*lcd_data_port = (p)`.
 *   - In the simulator: SIM_LCD_WRITE(p) expands to `sim_lcd_write(p)`,
 *     which calls our handler that updates the simulated LCD GRAM.
 *
 * The engine's framebuffer.c is the only file that needs to use this
 * macro — it's the only place that streams pixels. Other code that
 * touches lcd_data_port directly (none, in our engine) would still
 * work, just without being intercepted. */
#ifdef __cplusplus
extern "C" void sim_lcd_write(uint16_t pixel);
#else
void sim_lcd_write(uint16_t pixel);
#endif

#ifndef SIM_LCD_WRITE
#define SIM_LCD_WRITE(pixel) sim_lcd_write((pixel))
#endif

#ifdef __cplusplus
}
#endif
