/* src/main.cpp */
/*
 * cpcraft-port — main.cpp
 *
 * Entry point. Initializes the engine, runs the demo, returns.
 *
 * The demo (demo/demo.c) is the only thing here that knows what to render.
 * main.cpp just wires it up and provides the App metadata that Hollyhock-3
 * uses to display the app in its launcher.
 *
 * SIMULATOR NOTE: When building for the simulator, this file is compiled
 * with -Dmain=app_main (set in the simulator's Makefile target) so the
 * simulator's main_simulator.cpp can call app_main() after initializing
 * SDL. On hardware, main() is the real entry point as usual.
 */
#include <appdef.h>
#include <sdk/os/debug.h>
#include "engine/engine.h"
#include "demo/demo.h"

APP_NAME("CPCraft-Port")
APP_AUTHOR("cpcraft-port")
APP_DESCRIPTION("Fast-render engine base + demo for the ClassPad")
APP_VERSION("0.0.1")

int main(int /*argc*/, char ** /*argv*/, char ** /*envp*/)
{
    engine_init();
    demo_run();
    /* When the demo returns (user pressed MENU), wait for a key so the
     * Hollyhock-3 launcher can clean up. */
    Debug_WaitKey();
    return 0;
}
