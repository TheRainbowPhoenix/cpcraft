/* src/engine/input.c */
/*
 * cpcraft-port — engine/input.c
 *
 * Implementation of the GetInput()-based key poller.
 */
#include "input.h"
#include <sdk/os/input.h>

input_state_t input;

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
        case KEYCODE_0:            return EK_N0;
        case KEYCODE_1:            return EK_N1;
        case KEYCODE_2:            return EK_N2;
        case KEYCODE_3:            return EK_N3;
        case KEYCODE_4:            return EK_N4;
        case KEYCODE_5:            return EK_N5;
        case KEYCODE_6:            return EK_N6;
        case KEYCODE_7:            return EK_N7;
        case KEYCODE_8:            return EK_N8;
        case KEYCODE_9:            return EK_N9;
        case KEYCODE_PLUS:         return EK_PLUS;
        case KEYCODE_MINUS:        return EK_MINUS;
        case KEYCODE_DOT:          return EK_DOT;
        case KEYCODE_EXP:          return EK_EXP;
        case KEYCODE_DIVIDE:       return EK_DIV;
        case KEYCODE_TIMES:        return EK_MUL;
        default:                   return -1;
    }
}

void input_update(void)
{
    uint8_t prev_down[EK_COUNT];
    for (int i = 0; i < EK_COUNT; i++)
        prev_down[i] = input.down[i];

    for (;;)
    {
        struct Input_Event event __attribute__((aligned(4))) = {0};

        if (GetInput(&event, 0, 0x10) != 0)
            break;

        if (event.type == EVENT_NONE)
            break;

        if (event.type == EVENT_KEY)
        {
            const int dir = event.data.key.direction;
            const int is_pressed =
                (dir == KEY_PRESSED) || (dir == KEY_HELD);

            const int ek = keycode_to_ek(event.data.key.keyCode);
            if (ek >= 0)
                input.down[ek] = is_pressed ? 1 : 0;
        }
    }

    for (int i = 0; i < EK_COUNT; i++)
    {
        input.pressed[i]  = (input.down[i] && !prev_down[i]) ? 1 : 0;
        input.released[i] = (!input.down[i] && prev_down[i]) ? 1 : 0;
    }
}
