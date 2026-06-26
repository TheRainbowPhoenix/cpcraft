/*
 * cpcraft-port — engine/input.c
 *
 * Implementation of the GetInput()-based key poller.
 *
 * Pattern lifted from CP-Raycaster-Demo/src/main.cpp's playerInput():
 *
 *   struct Input_Event event __attribute__((aligned(4)));
 *   while (GetInput(&event, 0, 0x10) == 0 && event.type != EVENT_NONE) {
 *     if (event.type == EVENT_KEY) {
 *       int isPressed = (dir == KEY_PRESSED || dir == KEY_HELD);
 *       switch (event.data.key.keyCode) { ... }
 *     }
 *     Mem_Memset(&event, 0, sizeof(struct Input_Event));
 *   }
 *
 * Key insight: we DON'T reset the down[] flags before polling. The OS
 * sends KEY_HELD events for keys that stay down, so flags naturally
 * persist between frames. When a KEY_RELEASED event arrives, we clear
 * the flag. This is the same behavior as CP-Raycaster-Demo's key_*
 * variables.
 *
 * Edge detection (pressed[]/released[]) is computed by comparing the
 * post-poll down[] state with a snapshot taken before the poll.
 */
#include "input.h"
#include <sdk/os/input.h>

input_state_t input;

/* Map a KEYCODE_* value to our engine_key_t. Returns -1 if the keycode
 * isn't one we track. */
static int keycode_to_ek(int keycode)
{
    switch (keycode)
    {
        case KEYCODE_UP:           return EK_UP;
        case KEYCODE_DOWN:         return EK_DOWN;
        case KEYCODE_LEFT:         return EK_LEFT;
        case KEYCODE_RIGHT:        return EK_RIGHT;
        case KEYCODE_EXE:          return EK_EXE;
        case KEYCODE_BACKSPACE:    return EK_BACKSPACE;
        case KEYCODE_SHIFT:        return EK_SHIFT;
        case KEYCODE_POWER_CLEAR:  return EK_CLEAR;
        default:                   return -1;
    }
}

void input_update(void)
{
    /* Snapshot the pre-poll state for edge detection. */
    uint8_t prev_down[EK_COUNT];
    for (int i = 0; i < EK_COUNT; i++)
        prev_down[i] = input.down[i];

    /* Drain the OS event queue. GetInput(event, 0, 0x10) is non-blocking:
     * it returns 0 and fills `event` with the next event, or returns 0
     * with event.type == EVENT_NONE if the queue is empty.
     *
     * We use a fresh zero-initialized struct each iteration (matching
     * the CP-Raycaster pattern of Mem_Memset'ing between calls). */
    for (;;)
    {
        struct Input_Event event __attribute__((aligned(4))) = {0};

        if (GetInput(&event, 0, 0x10) != 0)
            break;  /* error — stop polling */

        if (event.type == EVENT_NONE)
            break;  /* queue empty */

        if (event.type == EVENT_KEY)
        {
            const int dir = event.data.key.direction;
            const int is_pressed =
                (dir == KEY_PRESSED) || (dir == KEY_HELD);

            const int ek = keycode_to_ek(event.data.key.keyCode);
            if (ek >= 0)
                input.down[ek] = is_pressed ? 1 : 0;
        }

        /* The struct is re-zeroed next iteration by the = {0} initializer. */
    }

    /* Compute edges by comparing post-poll state with pre-poll snapshot. */
    for (int i = 0; i < EK_COUNT; i++)
    {
        input.pressed[i]  = (input.down[i] && !prev_down[i]) ? 1 : 0;
        input.released[i] = (!input.down[i] && prev_down[i]) ? 1 : 0;
    }
}
