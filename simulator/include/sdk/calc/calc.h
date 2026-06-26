/* simulator/include/sdk/calc/calc.h */
/* simulator/include/sdk/calc/calc.h
 *
 * Drop-in replacement for the HHK3 SDK's <sdk/calc/calc.h>.
 *
 * Layout notes:
 *   - `vram` is declared as `extern uint16_t * const`, and the simulator
 *     runtime (simulator.cpp) points it at its heap-allocated framebuffer.
 *   - `width` and `height` are 320x528 (matches the ClassPad LCD).
 *
 * The engine's FB_W / FB_H constants are 160x264 because they're hardcoded
 * literals (see src/engine/framebuffer.h for the rationale), and that
 * matches width/2 and height/2 here.
 *
 * getKey() and the Keys1/Keys2 enums are mirrored from the real SDK so
 * any code that uses the bitmask-style API compiles unchanged. The
 * simulator doesn't actively drive these — most apps use Input_GetKeyState
 * from <sdk/os/input.h> instead.
 *
 * The real SDK uses `constexpr` as a keyword (C++23). Our simulator's
 * header is included from C sources (the engine is C), so we use plain
 * `static const` instead — same semantics for file-scope integer constants.
 */
#pragma once

#include <sdk/compiler.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Graphics — same as the real SDK. The simulator initializes `vram` to
 * point at its heap-allocated framebuffer in simulator_init(). */
extern uint16_t * const vram;
static const unsigned int width  = 320;
static const unsigned int height = 528;

void line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, uint16_t color);
void vline(unsigned int x, unsigned int y1, unsigned int y2, uint16_t color);
void triangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1,
              unsigned int x2, unsigned int y2, uint16_t colorFill, uint16_t colorLine);
void fillScreen(uint16_t color);

static inline __attribute__((pure)) uint16_t color(uint8_t R, uint8_t G, uint8_t B) {
    return (((R<<8) & 0xF800) |
            ((G<<3) & 0x07E0) |
            ((B>>3) & 0x001A));
}

static inline void setPixel(unsigned int x, unsigned int y, uint32_t c) {
    if(x < width && y < height)
        vram[width*y + x] = c;
}

/* Constructor/destructor hooks — the simulator provides no-op stubs. */
#if !defined(_MSC_VER)
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wprio-ctor-dtor"
#endif
#endif

__attribute__((constructor(99), used)) void calcInit(void);
__attribute__((destructor(99), used)) void calcExit(void);
#if !defined(_MSC_VER)
#pragma GCC diagnostic pop
#endif

/* Keyboard bitmask API (rarely used; the simulator doesn't drive it). */
void getKey(uint32_t *key1, uint32_t *key2);

enum Keys1 {
    KEY_SHIFT       = (int)0x80000000,
    KEY_CLEAR       = 0x00020000,
    KEY_BACKSPACE   = 0x00000080,
    KEY_LEFT        = 0x00004000,
    KEY_RIGHT       = 0x00008000,
    KEY_Z           = 0x00002000,
    KEY_POWER       = 0x00000040,
    KEY_DIVIDE      = 0x40000000,
    KEY_MULTIPLY    = 0x20000000,
    KEY_SUBTRACT    = 0x10000000,
    KEY_ADD         = 0x08000000,
    KEY_EXE         = 0x04000000,
    KEY_EXP         = 0x00000004,
    KEY_3           = 0x00000008,
    KEY_6           = 0x00000010,
    KEY_9           = 0x00000020,
};

enum Keys2 {
    KEY_KEYBOARD        = (int)0x80000000,
    KEY_UP              = 0x00800000,
    KEY_DOWN            = 0x00400000,
    KEY_EQUALS          = 0x00000080,
    KEY_X               = 0x00000040,
    KEY_Y               = 0x40000000,
    KEY_LEFT_BRACKET    = 0x00000020,
    KEY_RIGHT_BRACKET   = 0x00000010,
    KEY_COMMA           = 0x00000008,
    KEY_NEGATIVE        = 0x00000004,
    KEY_0               = 0x04000000,
    KEY_DOT             = 0x00040000,
    KEY_1               = 0x08000000,
    KEY_2               = 0x00080000,
    KEY_4               = 0x10000000,
    KEY_5               = 0x00100000,
    KEY_7               = 0x20000000,
    KEY_8               = 0x00200000,
};

#ifdef __cplusplus
}  /* end extern "C" */

/* C++ overloads — these can't go inside extern "C". */
static inline void getKey(Keys1 *key1, Keys2 *key2) {
    uint32_t k1, k2;
    getKey(&k1, &k2);
    *key1 = static_cast<Keys1>(k1);
    *key2 = static_cast<Keys2>(k2);
}

static inline __attribute__((pure)) bool testKey(uint32_t key1, uint32_t, Keys1 key) {
    return key1 & key;
}

static inline __attribute__((pure)) bool testKey(uint32_t, uint32_t key2, Keys2 key) {
    return key2 & key;
}
#endif
