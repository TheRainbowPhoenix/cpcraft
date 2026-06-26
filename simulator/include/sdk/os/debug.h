/* simulator/include/sdk/os/debug.h */
/* simulator/include/sdk/os/debug.h
 *
 * Drop-in replacement for the HHK3 SDK's <sdk/os/debug.h>.
 *
 * The simulator implements Debug_Printf by rendering into a small overlay
 * at the top of the LCD framebuffer using the same 8x12 system font as
 * the real OS. Debug_WaitKey pumps SDL events until a key is pressed.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Debug font metrics. In the real SDK these are `constexpr unsigned int`
 * (C++ only). In the simulator we use `static const unsigned int` so it
 * compiles in both C and C++. */
static const unsigned int debug_char_width  = 8;
static const unsigned int debug_char_height = 12;
static const unsigned int debug_line_height = 14;
static const unsigned int debug_max_columns = 40;
static const unsigned int debug_max_rows    = 37;
extern uint16_t *DEBUG_FONTBASE;

extern void (*Debug_GetCursorPosition)(unsigned int *x, unsigned int *y);
extern void (*Debug_Printf)(unsigned int x, unsigned int y, bool invert, int zero, const char *format, ...);
extern void (*Debug_PrintNumberHex_Byte)(uint8_t value, unsigned int x, unsigned int y);
extern void (*Debug_PrintNumberHex_Dword)(uint32_t value, unsigned int x, unsigned int y);
extern void (*Debug_PrintNumberHex_Nibble)(uint8_t value, unsigned int x, unsigned int y);
extern void (*Debug_PrintNumberHex_Word)(uint16_t value, unsigned int x, unsigned int y);
extern bool (*Debug_PrintString)(const char *string, bool invert);
extern int (*Debug_SetCursorPosition)(unsigned int x, unsigned int y);
extern int (*Debug_WaitKey)();

#ifdef __cplusplus
}
#endif
