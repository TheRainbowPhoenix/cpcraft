/*
 * cpcraft-port — demo/demo.h
 *
 * Public API of the demo. There's only one entry point: demo_run().
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Run the demo. Returns when the user presses the MENU key (POWER/CLEAR). */
void demo_run(void);

#ifdef __cplusplus
}
#endif
