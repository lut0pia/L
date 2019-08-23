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
  __pragma(warning(disable: 4701))
#define L_POP_NO_WARNINGS __pragma(warning(pop))
#else
#define L_PUSH_NO_WARNINGS \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wall\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
#define L_POP_NO_WARNINGS _Pragma("GCC diagnostic pop")
#endif
