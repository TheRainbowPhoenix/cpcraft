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
 *   EK_UP, EK_DOWN, EK_LEFT, EK_RIGHT  — D-pad
 *   EK_EXE                              — EXE key (primary action)
 *   EK_BACKSPACE                        — Backspace (secondary action)
 *   EK_SHIFT                            — Shift (modifier)
 *   EK_CLEAR                            — Power/Clear (modifier + exit)
 *
 * Exit combo: when both EK_SHIFT and EK_CLEAR are down simultaneously,
 * input_exit_requested() returns true. The demo checks this each frame
 * and returns from demo_run() when it fires.
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
    EK_EXE       = 4,   /* EXE key — primary action */
    EK_BACKSPACE = 5,   /* Backspace — secondary action */
    EK_SHIFT     = 6,   /* Shift — modifier, used for exit combo */
    EK_CLEAR     = 7,   /* Power/Clear — modifier, used for exit combo */
    EK_COUNT
} engine_key_t;

/* Snapshot of the keyboard. Updated by input_update(). */
typedef struct {
    uint8_t down[EK_COUNT];       /* currently down? */
    uint8_t pressed[EK_COUNT];    /* went down this frame? (edge) */
    uint8_t released[EK_COUNT];   /* went up this frame? (edge) */
} input_state_t;

/* Global input state — single instance, no malloc. */
extern input_state_t input;

/* Update the global input state. Call once per frame BEFORE reading keys.
 *
 * Drains the OS's GetInput() event queue and updates the down/pressed/
 * released arrays. This is the only function that talks to the OS. */
void input_update(void);

/* Convenience predicates. */
static inline bool input_down(engine_key_t k)     { return input.down[k]; }
static inline bool input_pressed(engine_key_t k)  { return input.pressed[k]; }
static inline bool input_released(engine_key_t k) { return input.released[k]; }

/* Returns true if the Shift+Clear combo is currently held. The demo uses
 * this to exit cleanly — same pattern as CP-Raycaster-Demo. */
static inline bool input_exit_requested(void)
{
    return input.down[EK_SHIFT] && input.down[EK_CLEAR];
}

#ifdef __cplusplus
}
#endif
