/* simulator/src/main_simulator.cpp */
/* simulator/src/main_simulator.cpp
 *
 * Entry point for the simulator build.
 *
 * The app's real main() (in src/main.cpp) is renamed to app_main() at
 * compile time via -Dmain=app_main in the simulator's Makefile. This
 * file provides the real main() that:
 *   1. Calls simulator_init() to set up SDL2 and the LCD framebuffer.
 *   2. Calls app_main() (the engine's entry point).
 *
 * Build (see simulator/Makefile for the full command):
 *   g++ -Dmain=app_main -I simulator/include -I src \
 *       -c src/main.cpp -o obj/main.app.o
 *   g++ -I simulator/include -I src \
 *       -c simulator/src/main_simulator.cpp -o obj/main_sim.o
 *   ... link everything together with -lSDL2 ...
 */
#include <appdef.h>

/* Forward declarations.
 *
 * simulator_init() is declared extern "C" in simulator.cpp — keep the
 * same linkage here so the symbols match at link time.
 *
 * app_main() is the engine's main(), renamed from `main` via
 * -Dmain=app_main at compile time. The engine's main.cpp is C++ (compiled
 * with g++), so app_main has C++ linkage and a mangled symbol name — we
 * declare it WITHOUT extern "C". */
extern "C" void simulator_init();

int app_main(int argc, char **argv, char **envp);

int main(int argc, char **argv, char **envp)
{
    /* Initialize SDL and the simulator's LCD framebuffer BEFORE the
     * engine's app_main() runs (the engine writes to the LCD on init). */
    simulator_init();

    /* Hand off to the app's real main (renamed to app_main via -D). */
    return app_main(argc, argv, envp);
}
