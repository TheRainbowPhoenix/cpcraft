/* src/engine/engine.c */
/*
 * cpcraft-port — engine/engine.c
 *
 * One-stop init.
 */
#include "engine.h"

void engine_init(void)
{
    fb_init();
    tex_init();
    world_init();
    player_init();
    block_render_init();
    camera_init();
}
