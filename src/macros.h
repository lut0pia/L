#pragma once

// Forbids class from being copied
#define L_NOCOPY(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete;

// Allows true stringify
#define L_STRINGIFY(n) L_STRINGIFY_(n)
#define L_STRINGIFY_(n) #n

#define L_ONCE do{static bool DONE_ONCE(false);if(DONE_ONCE) return;DONE_ONCE = true;}while(false)
#define L_DO_ONCE static bool DONE_ONCE(false);if(!DONE_ONCE && (DONE_ONCE = true))

#define L_COUNT_OF(a) (sizeof(a)/sizeof(*a))
