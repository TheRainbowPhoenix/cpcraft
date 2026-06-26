/* test/sdk-stub/sdk/calc/calc.h
 *
 * Stub of the real <sdk/calc/calc.h> for host-side testing.
 *
 * The real header declares `vram` as a `static uint16_t * const` pointing
 * at 0x8c000000 and `width`/`height` as `static const unsigned int`.
 * We mirror that so the engine compiles unchanged.
 *
 * Note: on the host, writing to vram at 0x8c000000 will segfault, so the
 * test harness redirects fb_vram to a real buffer before calling any
 * engine function.
 */
#pragma once
#include <stdint.h>

static uint16_t * const vram = (uint16_t *)0x8c000000;
static const unsigned int width  = 320;
static const unsigned int height = 528;
