/*
 * cpcraft-port — engine/input.c
 *
 * Implementation of the input poller.
 *
 * Map: we keep a small fixed table of (logical key -> scancode). The order
 * matches engine_key_t so we can index by enum.
 */
#include "input.h"
#include <string.h>

input_state_t input;

static const enum Input_Scancode k_scancode[EK_COUNT] = {
    [EK_UP]    = ScancodeUp,
    [EK_DOWN]  = ScancodeDown,
    [EK_LEFT]  = ScancodeLeft,
    [EK_RIGHT] = ScancodeRight,
    [EK_A]     = ScancodeEXE,
    [EK_B]     = ScancodeBackspace,
    [EK_MENU]  = ScancodeClear,
};

void input_update(void)
{
    static uint8_t prev[EK_COUNT];

    /* Save previous frame's down-state before we overwrite it. */
    memcpy(prev, input.down, EK_COUNT);

    for (int i = 0; i < EK_COUNT; i++)
    {
        const bool now = Input_GetKeyState(k_scancode[i]);
        input.down[i]     = now ? 1 : 0;
        input.pressed[i]  = (now && !prev[i])  ? 1 : 0;
        input.released[i] = (!now && prev[i])  ? 1 : 0;
    }
}
