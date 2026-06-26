#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
  #define __attribute__(x)
  #define SDK_ALWAYS_INLINE __forceinline
  #define SDK_GCC_DIAG_PUSH
  #define SDK_GCC_DIAG_POP
  #define SDK_GCC_DIAG_IGNORED(x)
#else
  #define SDK_ALWAYS_INLINE inline __attribute__((always_inline))
  #define SDK_GCC_DIAG_PUSH _Pragma("GCC diagnostic push")
  #define SDK_GCC_DIAG_POP _Pragma("GCC diagnostic pop")
  #define SDK_GCC_DIAG_IGNORED(x) _Pragma(#x)
#endif