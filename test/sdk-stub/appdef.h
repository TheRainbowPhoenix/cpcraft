/* test/sdk-stub/appdef.h
 *
 * Stub of the real <appdef.h> for host-side testing.
 *
 * The real header defines APP_NAME/APP_AUTHOR/etc. as macros that emit
 * ELF note sections; we don't need those for the host-side test harness,
 * so we just define them to nothing.
 */
#pragma once

#define APP_NAME(x)
#define APP_AUTHOR(x)
#define APP_DESCRIPTION(x)
#define APP_VERSION(x)
