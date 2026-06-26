/*
 * cpcraft-port — engine/input.h
 *
 * Tiny key-state polling helper for the engine.
 *
 * The SDK's Input_GetKeyState() takes a scancode and returns whether the key
 * is currently down. That's fine for ad-hoc checks but CPCraft-style engines
 * need "just pressed" / "just released" edges, so we keep two snapshots and
 * expose simple predicates on top.
 *
 * The set of keys tracked is intentionally minimal — we only poll what the
 * engine needs (D-pad + action keys). Each poll costs one syscall per key,
 * and the OS key scan isn't free, so keeping this list short matters.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <sdk/os/input.h>

/* Logical key enum — the engine never sees raw scancodes.
 *
 * NOTE: prefixed `EK_` (Engine Key) to avoid colliding with the SDK's
 * own `KEY_UP` / `KEY_DOWN` / `KEY_LEFT` / `KEY_RIGHT` constants in
 * <sdk/calc/calc.h>. The SDK's KEY_* are bitmask values for the
 * getKey() bitmask API; ours are small indices into input_state_t.
 * Keep them in different namespaces to avoid subtle bugs. */
typedef enum {
    EK_UP    = 0,
    EK_DOWN  = 1,
    EK_LEFT  = 2,
    EK_RIGHT = 3,
    EK_A     = 4,   /* EXE */
    EK_B     = 5,   /* BACKSPACE */
    EK_MENU  = 6,   /* POWER/CLEAR — used to exit the demo */
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

/* Update the global input state. Call once per frame BEFORE reading keys. */
void input_update(void);

/* Convenience predicates. */
static inline bool input_down(engine_key_t k)    { return input.down[k]; }
static inline bool input_pressed(engine_key_t k) { return input.pressed[k]; }
static inline bool input_released(engine_key_t k){ return input.released[k]; }

#ifdef __cplusplus
}
#endif
