#include <sdk/os/input.h>
#include <string.h>
#include "controls.h"

// We map ClassPad scancodes to the Prizm-style basic_keycodes used by the game
// This is just to keep the game logic mostly unchanged.

static uint8_t key_states[256];
static uint8_t last_key_states[256];

// Helper to map basic_keycode to scancode
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

        // Directional keys
        case KEY_PRGM_UP: return ScancodeUp;
        case KEY_PRGM_DOWN: return ScancodeDown;
        case KEY_PRGM_LEFT: return ScancodeLeft;
        case KEY_PRGM_RIGHT: return ScancodeRight;

        // Other keys
        case KEY_PRGM_EXIT: return ScancodeClear; // Mapping EXIT to Clear
        case KEY_PRGM_MENU: return ScancodeKeyboard; // Mapping MENU to Keyboard
        case KEY_PRGM_SHIFT: return ScancodeShift;
        case KEY_PRGM_OPTN: return ScancodeEXP; // Mapping OPTN to EXP
        case KEY_PRGM_ALPHA: return ScancodeNegative; // Mapping ALPHA to (-)
        case KEY_VARS: return ScancodeComma; // Mapping VARS to Comma

        // F keys - mapping them to something sensible since CP doesn't have them
        case KEY_PRGM_F1: return ScancodeOpenParenthesis;
        case KEY_PRGM_F2: return ScancodeCloseParenthesis;
        case KEY_PRGM_F3: return Scancode7; // Not ideal, but needs some mapping
        case KEY_PRGM_F4: return Scancode8;
        case KEY_PRGM_F5: return Scancode9;
        case KEY_PRGM_F6: return ScancodeDivide;

        default: return (enum Input_Scancode)0;
    }
}

void keyupdate(void) {
    memcpy(last_key_states, key_states, 256);

    // We update the states for the keys we care about
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
        enum Input_Scancode sc = map_to_scancode(k);
        if (sc != 0) {
            key_states[k] = Input_GetKeyState(sc);
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
