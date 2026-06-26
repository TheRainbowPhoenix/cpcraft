/* test/sdk-stub/appdef.h */
/* simulator/include/appdef.h */
/* simulator/include/appdef.h
 *
 * Drop-in replacement for the HHK3 SDK's <appdef.h>.
 *
 * The real header defines APP_NAME/APP_AUTHOR/etc. as macros that emit
 * ELF note sections for the Hollyhock-3 launcher to read. The simulator
 * doesn't need those, so we define them to nothing.
 *
 * Everything else (LCD, input, debug, calc) is implemented in the sibling
 * sdk/ headers below + simulator.cpp.
 */
#pragma once

#define APP_NAME(x)
#define APP_AUTHOR(x)
#define APP_DESCRIPTION(x)
#define APP_VERSION(x)
