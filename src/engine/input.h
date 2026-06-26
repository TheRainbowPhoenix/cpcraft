/* src/engine/input.h */
/*
 * cpcraft-port — engine/input.h
 *
 * Key-state polling helper using the ClassPad's GetInput() event API.
 *
 * Design: matches the CP-Raycaster-Demo pattern. Each frame, input_update()
 * drains the OS's event queue via GetInput(). For each KEY event, we update
 * the corresponding flag. We DON'T reset flags between events — the OS
 * sends KEY_HELD events for keys that stay down, so flags naturally persist.
 * When a KEY_RELEASED event arrives, the flag clears.
 *
 * This is more reliable than Input_GetKeyState() (which can miss held keys)
 * and is the same pattern CP-Raycaster-Demo uses to detect Shift+Clear
 * as an "exit" combo.
 *
 * Keys tracked:
 *   EK_UP, EK_DOWN, EK_LEFT, EK_RIGHT  — D-pad (camera look)
 *   EK_EXE                              — EXE key (primary action)
 *   EK_BACKSPACE                        — Backspace (secondary action)
 *   EK_SHIFT                            — Shift (modifier)
 *   EK_CLEAR                            — Power/Clear (modifier + exit)
 *   EK_N0..EK_N9                        — Number keys 0-9
 *   EK_PLUS, EK_MINUS                   — + and - keys
 *   EK_DOT, EK_EXP                      — . and EXP keys
 *   EK_DIV, EK_MUL                      — / and * keys
 *
 * Exit combo: when both EK_SHIFT and EK_CLEAR are down simultaneously,
 * input_exit_requested() returns true.
 *
 * Numpad controls (CPCraft-style):
 *   8 = forward, 5 = backward, 4 = strafe left, 6 = strafe right
 *   2 = jump (or backwards depending on context)
 *   7 = camera left, 9 = camera right
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Logical key enum. Prefixed `EK_` to avoid colliding with the SDK's
 * own KEY_* bitmask constants in <sdk/calc/calc.h>. */
typedef enum {
    EK_UP        = 0,
    EK_DOWN      = 1,
    EK_LEFT      = 2,
    EK_RIGHT     = 3,
    EK_EXE       = 4,
    EK_BACKSPACE = 5,
    EK_SHIFT     = 6,
    EK_CLEAR     = 7,
    /* Number keys */
    EK_N0        = 8,
    EK_N1        = 9,
    EK_N2        = 10,
    EK_N3        = 11,
    EK_N4        = 12,
    EK_N5        = 13,
    EK_N6        = 14,
    EK_N7        = 15,
    EK_N8        = 16,
    EK_N9        = 17,
    /* Math keys */
    EK_PLUS      = 18,
    EK_MINUS     = 19,
    EK_DOT       = 20,
    EK_EXP       = 21,
    EK_DIV       = 22,
    EK_MUL       = 23,
    EK_COUNT
} engine_key_t;

/* Snapshot of the keyboard. Updated by input_update(). */
typedef struct {
    uint8_t down[EK_COUNT];
    uint8_t pressed[EK_COUNT];
    uint8_t released[EK_COUNT];
} input_state_t;

extern input_state_t input;

void input_update(void);

static inline bool input_down(engine_key_t k)     { return input.down[k]; }
static inline bool input_pressed(engine_key_t k)  { return input.pressed[k]; }
static inline bool input_released(engine_key_t k) { return input.released[k]; }

static inline bool input_exit_requested(void)
{
    return input.down[EK_SHIFT] && input.down[EK_CLEAR];
}

#ifdef __cplusplus
}
#endif
