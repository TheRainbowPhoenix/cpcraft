#ifndef MACROS_H
#define MACROS_H

#define APPNAME_STRING "CPDoom"

#define DIRECTORY_DOOM  "\\fls0\\" APPNAME_STRING
#define DIRECTORY_WAD   DIRECTORY_DOOM "\\wad"

#define RETURN_FAILURE(val) \
    if (val) \
    { \
        return -1; \
    }   

#endif

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)