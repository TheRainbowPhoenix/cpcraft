/* test/sdk-stub/sdk/os/debug.h
 *
 * Stub of the real <sdk/os/debug.h> for host-side testing.
 *
 * The real header provides function pointers to the OS's debug print and
 * key-wait routines. For the host test we just declare them as extern
 * function pointers — the test harness (test/native_test.c) provides
 * no-op definitions.
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int  (*Debug_WaitKey)();
extern void (*Debug_Printf)(unsigned int x, unsigned int y, bool invert, int zero, const char *format, ...);

#ifdef __cplusplus
}
#endif
