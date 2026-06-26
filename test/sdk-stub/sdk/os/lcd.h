/* test/sdk-stub/sdk/os/lcd.h */
/*
 * Stub of the real HHK3 SDK's <sdk/os/lcd.h> for host-side testing.
 *
 * This is the SIMPLE version for the native test harness — it does NOT
 * define SIM_LCD_WRITE (the engine's framebuffer.h will define it as
 * `*lcd_data_port = pixel` which writes to a dummy sink in the test).
 *
 * The simulator (simulator/include/sdk/os/lcd.h) has a more complete
 * version that defines SIM_LCD_WRITE to call sim_lcd_write().
 */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LCD_Command {
    COMMAND_SET_X_WINDOW = 0x2A,
    COMMAND_SET_Y_WINDOW = 0x2B,
    COMMAND_PREPARE_FOR_DRAW_DATA = 0x2C,
    COMMAND_READ_DRAW_DATA = 0x2E
};

extern volatile uint16_t *lcd_data_port;

extern void (*LCD_SetDrawingBounds)(unsigned int xstart, unsigned int xend,
                                    unsigned int ystart, unsigned int yend);
extern void (*LCD_SendCommandU)(uint16_t command);

static inline void LCD_SendCommand(enum LCD_Command command) {
    LCD_SendCommandU((uint16_t)command);
}

extern void (*LCD_Refresh)();

/* VRAM address accessor — the engine uses this in fb_init() to get a
 * pointer to the VRAM buffer. On hardware this returns 0x8c000000. */
extern uint16_t *(*_FP_LCD_GetVRAMAddress)();
static inline uint16_t *LCD_GetVRAMAddress() { return _FP_LCD_GetVRAMAddress(); }

#define RGB_TO_RGB565(r, g, b) ( \
    ((r & 0x1F) << 11) | \
    ((g & 0x3F) << 5)  | \
    (b & 0x1F) \
)

#ifdef __cplusplus
}
#endif
