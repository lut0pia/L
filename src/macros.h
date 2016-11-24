#pragma once

#include <cstdio>
#include <cstdlib>

#ifdef L_UNIX
# include <signal.h>
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define L_X86_64
#elif defined(__i386) || defined(_M_IX86)
#define L_X86_32
#endif

/*
#define L_NoCopy(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete;
*/

// Forbids class from being copied
#define L_NOCOPY(class) \
  class(const class&); \
  class& operator=(const class&);

// Allows true stringify
#define L_STRINGIFY(n) L_STRINGIFY_(n)
#define L_STRINGIFY_(n) #n

// Debugging macros
#ifdef L_DEBUG
#define L_DEBUGONLY(...) do{__VA_ARGS__;}while(false)
#else
#define L_DEBUGONLY(...) ((void)0)
#endif

#if defined _MSC_VER
# define L_BREAKPOINT __debugbreak()
#elif defined __GNUC__
# define L_BREAKPOINT raise(SIGTRAP)
#endif

#define L_ERROR(msg,...) do{fprintf(stderr,"Error in %s:%d:\n" msg "\n",__FILE__,__LINE__,__VA_ARGS__);L_BREAKPOINT;exit(-1);}while(false)
#define L_ASSERT(exp) L_DEBUGONLY(if(!(exp))L_ERROR("%s is false",#exp))

#define L_ONCE do{static bool DONE_ONCE(false);if(DONE_ONCE) return;DONE_ONCE = true;}while(false)
#define L_DO_ONCE static bool DONE_ONCE(false);if(!DONE_ONCE && (DONE_ONCE = true))
