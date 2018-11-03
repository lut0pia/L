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
