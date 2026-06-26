/* test/sdk-stub/sdk/os/input.h
 *
 * Stub of the real <sdk/os/input.h> for host-side testing.
 *
 * Only the scancodes used by engine/input.c are listed; the real header
 * has more. The Input_GetKeyState function is declared extern — the test
 * harness provides a stub that always returns false.
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum Input_Scancode {
    ScancodeKeyboard        = (7 << 8) | 5,
    ScancodeShift           = (7 << 8) | 1,
    ScancodeBackspace       = (7 << 8) | 2,
    ScancodeClear           = (1 << 8) | 0,
    ScancodeUp              = (7 << 8) | 4,
    ScancodeDown            = (6 << 8) | 4,
    ScancodeLeft            = (6 << 8) | 3,
    ScancodeRight           = (7 << 8) | 3,
    ScancodeEquals          = (7 << 8) | 6,
    ScancodeX               = (6 << 8) | 6,
    ScancodeY               = (6 << 8) | 5,
    ScancodeZ               = (5 << 8) | 3,
    ScancodePower           = (6 << 8) | 2,
    ScancodeDivide          = (6 << 8) | 1,
    ScancodeOpenParenthesis = (5 << 8) | 6,
    Scancode7               = (5 << 8) | 5,
    Scancode8               = (5 << 8) | 4,
    Scancode9               = (5 << 8) | 2,
    ScancodeTimes           = (5 << 8) | 1,
    ScancodeCloseParenthesis= (4 << 8) | 6,
    Scancode4               = (4 << 8) | 5,
    Scancode5               = (4 << 8) | 4,
    Scancode6               = (4 << 8) | 2,
    ScancodeMinus           = (4 << 8) | 1,
    ScancodeComma           = (3 << 8) | 6,
    Scancode1               = (3 << 8) | 5,
    Scancode2               = (3 << 8) | 4,
    Scancode3               = (3 << 8) | 2,
    ScancodePlus            = (3 << 8) | 1,
    ScancodeNegative        = (2 << 8) | 6,
    Scancode0               = (2 << 8) | 5,
    ScancodeDot             = (2 << 8) | 4,
    ScancodeEXP             = (2 << 8) | 2,
    ScancodeEXE             = (2 << 8) | 1
};

bool Input_GetKeyState(enum Input_Scancode scanCode);

#ifdef __cplusplus
}
#endif
