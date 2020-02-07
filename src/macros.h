#pragma once

// Forbids class from being copied
#define L_NOCOPY(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete;

// Allows true stringify
#define L_STRINGIFY(n) L_STRINGIFY_(n)
#define L_STRINGIFY_(n) #n

#define L_CONCAT(a,b) L_CONCAT_(a,b)
#define L_CONCAT_(a,b) a##b

#define L_ONCE do{static bool DONE_ONCE(false);if(DONE_ONCE) return;DONE_ONCE = true;}while(false)

#define L_COUNT_OF(a) (sizeof(a)/sizeof(*a))

// Compiler-specific pragmas
#if defined _MSC_VER
#define L_PUSH_NO_WARNINGS __pragma(warning(push, 0)) \
  __pragma(warning(disable: 4577)) \
  __pragma(warning(disable: 4701)) \
  __pragma(warning(disable: 4702)) \
  __pragma(warning(disable: 4703))
#define L_POP_NO_WARNINGS __pragma(warning(pop))
#else
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#define L_GCC4_7_IGNORE_WARNINGS \
  _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#else
#define L_GCC4_7_IGNORE_WARNINGS
#endif
#if __GNUC__ >= 7
#define L_GCC7_IGNORE_WARNINGS \
  _Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")
#else
#define L_GCC7_IGNORE_WARNINGS
#endif
#define L_PUSH_NO_WARNINGS \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wall\"") \
  _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
  _Pragma("GCC diagnostic ignored \"-Wextra\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
  L_GCC4_7_IGNORE_WARNINGS \
  L_GCC7_IGNORE_WARNINGS
#define L_POP_NO_WARNINGS _Pragma("GCC diagnostic pop")
#endif
