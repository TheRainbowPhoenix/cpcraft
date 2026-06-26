/* test/sdk-stub/sdk/os/lcd.h
 *
 * Stub of the real <sdk/os/lcd.h> for host-side testing.
 *
 * Key differences from the real header:
 *   - lcd_data_port is declared non-const here so the test harness can
 *     redirect it to a real buffer (the real SDK has it as
 *     `volatile uint16_t *const` because the port never moves on hw).
 *   - LCD_SetDrawingBounds etc. are plain function pointers (the test
 *     harness provides no-op definitions).
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

/* Non-const on host so the test can redirect. */
extern volatile uint16_t *lcd_data_port;

extern void (*LCD_SetDrawingBounds)(unsigned int xstart, unsigned int xend,
                                    unsigned int ystart, unsigned int yend);
extern void (*LCD_SendCommandU)(uint16_t command);

static inline void LCD_SendCommand(enum LCD_Command command) {
    LCD_SendCommandU((uint16_t)command);
}

extern void (*LCD_Refresh)();

#define RGB_TO_RGB565(r, g, b) ( \
    ((r & 0x1F) << 11) | \
    ((g & 0x3F) << 5)  | \
    (b & 0x1F) \
)

#ifdef __cplusplus
}
#endif
