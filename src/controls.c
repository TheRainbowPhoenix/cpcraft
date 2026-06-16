#include <sdk/os/input.h>
#include <string.h>
#include "controls.h"

// We track a small subset of Prizm keycodes that we map to ClassPad scancodes.
// basic_keycodes can be large, so we use a sparse mapping or a larger array.
// For simplicity and since fxcraft only uses a few, we'll use an array of 256
// and ensure we don't index out of bounds.

static uint8_t key_states[256];
static uint8_t last_key_states[256];

static enum Input_Scancode map_to_scancode(int basic_keycode) {
    switch (basic_keycode) {
        case KEY_EXE: return ScancodeEXE;
        case KEY_PLUS: return ScancodePlus;
        case KEY_MINUS: return ScancodeMinus;
        case KEY_PRGM_0: return Scancode0;
        case KEY_PRGM_1: return Scancode1;
        case KEY_PRGM_2: return Scancode2;
        case KEY_PRGM_3: return Scancode3;
        case KEY_PRGM_4: return Scancode4;
        case KEY_PRGM_5: return Scancode5;
        case KEY_PRGM_6: return Scancode6;
        case KEY_PRGM_7: return Scancode7;
        case KEY_PRGM_8: return Scancode8;
        case KEY_PRGM_9: return Scancode9;
        case KEY_PRGM_UP: return ScancodeUp;
        case KEY_PRGM_DOWN: return ScancodeDown;
        case KEY_PRGM_LEFT: return ScancodeLeft;
        case KEY_PRGM_RIGHT: return ScancodeRight;
        case KEY_PRGM_EXIT: return ScancodeClear;
        case KEY_PRGM_MENU: return ScancodeKeyboard;
        case KEY_PRGM_SHIFT: return ScancodeShift;
        case KEY_PRGM_OPTN: return ScancodeEXP;
        case KEY_PRGM_ALPHA: return ScancodeNegative;
        case KEY_VARS: return ScancodeComma;
        case KEY_PRGM_F1: return ScancodeOpenParenthesis;
        case KEY_PRGM_F2: return ScancodeCloseParenthesis;
        case KEY_PRGM_F3: return Scancode7;
        case KEY_PRGM_F4: return Scancode8;
        case KEY_PRGM_F5: return Scancode9;
        case KEY_PRGM_F6: return ScancodeDivide;
        default: return (enum Input_Scancode)0;
    }
}

void keyupdate(void) {
    memcpy(last_key_states, key_states, 256);

    const int keys_to_track[] = {
        KEY_EXE, KEY_PLUS, KEY_MINUS, KEY_VARS,
        KEY_PRGM_0, KEY_PRGM_1, KEY_PRGM_2, KEY_PRGM_3, KEY_PRGM_4,
        KEY_PRGM_5, KEY_PRGM_6, KEY_PRGM_7, KEY_PRGM_8, KEY_PRGM_9,
        KEY_PRGM_UP, KEY_PRGM_DOWN, KEY_PRGM_LEFT, KEY_PRGM_RIGHT,
        KEY_PRGM_EXIT, KEY_PRGM_MENU, KEY_PRGM_SHIFT, KEY_PRGM_OPTN, KEY_PRGM_ALPHA,
        KEY_PRGM_F1, KEY_PRGM_F2, KEY_PRGM_F3, KEY_PRGM_F4, KEY_PRGM_F5, KEY_PRGM_F6
    };

    for (size_t i = 0; i < sizeof(keys_to_track)/sizeof(keys_to_track[0]); i++) {
        int k = keys_to_track[i];
        if (k >= 0 && k < 256) {
            enum Input_Scancode sc = map_to_scancode(k);
            if (sc != 0) {
                key_states[k] = Input_GetKeyState(sc);
            }
        }
    }
}

int keydownlast(int basic_keycode) {
    if (basic_keycode < 0 || basic_keycode >= 256) return 0;
    return key_states[basic_keycode];
}

int keydownhold(int basic_keycode) {
    if (basic_keycode < 0 || basic_keycode >= 256) return 0;
    return last_key_states[basic_keycode];
}
