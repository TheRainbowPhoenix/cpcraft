#include <sdk/os/input.h>
#include <string.h>
#include "controls.h"

typedef struct {
    int keycode;
    enum Input_Scancode scancode;
    uint8_t state;
    uint8_t last_state;
} KeyMap;

static KeyMap key_maps[] = {
    {KEY_EXE, ScancodeEXE, 0, 0},
    {KEY_PLUS, ScancodePlus, 0, 0},
    {KEY_MINUS, ScancodeMinus, 0, 0},
    {KEY_VARS, ScancodeComma, 0, 0},
    {KEY_PRGM_0, Scancode0, 0, 0},
    {KEY_PRGM_1, Scancode1, 0, 0},
    {KEY_PRGM_2, Scancode2, 0, 0},
    {KEY_PRGM_3, Scancode3, 0, 0},
    {KEY_PRGM_4, Scancode4, 0, 0},
    {KEY_PRGM_5, Scancode5, 0, 0},
    {KEY_PRGM_6, Scancode6, 0, 0},
    {KEY_PRGM_7, Scancode7, 0, 0},
    {KEY_PRGM_8, Scancode8, 0, 0},
    {KEY_PRGM_9, Scancode9, 0, 0},
    {KEY_PRGM_UP, ScancodeUp, 0, 0},
    {KEY_PRGM_DOWN, ScancodeDown, 0, 0},
    {KEY_PRGM_LEFT, ScancodeLeft, 0, 0},
    {KEY_PRGM_RIGHT, ScancodeRight, 0, 0},
    {KEY_PRGM_EXIT, ScancodeClear, 0, 0},
    {KEY_PRGM_MENU, ScancodeKeyboard, 0, 0},
    {KEY_PRGM_SHIFT, ScancodeShift, 0, 0},
    {KEY_PRGM_OPTN, ScancodeEXP, 0, 0},
    {KEY_PRGM_ALPHA, ScancodeNegative, 0, 0},
    {KEY_PRGM_F1, ScancodeOpenParenthesis, 0, 0},
    {KEY_PRGM_F2, ScancodeCloseParenthesis, 0, 0},
    {KEY_PRGM_F3, Scancode7, 0, 0},
    {KEY_PRGM_F4, Scancode8, 0, 0},
    {KEY_PRGM_F5, Scancode9, 0, 0},
    {KEY_PRGM_F6, ScancodeDivide, 0, 0}
};

#define KEY_MAP_COUNT (sizeof(key_maps)/sizeof(key_maps[0]))

void keyupdate(void) {
    for (size_t i = 0; i < KEY_MAP_COUNT; i++) {
        key_maps[i].last_state = key_maps[i].state;
        key_maps[i].state = Input_GetKeyState(key_maps[i].scancode);
    }
}

int keydownlast(int basic_keycode) {
    for (size_t i = 0; i < KEY_MAP_COUNT; i++) {
        if (key_maps[i].keycode == basic_keycode) return key_maps[i].state;
    }
    return 0;
}

int keydownhold(int basic_keycode) {
    for (size_t i = 0; i < KEY_MAP_COUNT; i++) {
        if (key_maps[i].keycode == basic_keycode) return key_maps[i].last_state;
    }
    return 0;
}
