#ifndef DEF_L_macros
#define DEF_L_macros

#define typeof __typeof__ // static operator replaced by type name of expression

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
# define L_BREAKPOINT DebugBreak()
#elif defined __GNUC__
# define L_BREAKPOINT raise(SIGTRAP)
#endif

#define L_ERROR(msg) do{fprintf(stderr,"Error: "L_STRINGIFY(msg)" in %s:%d\n",__FILE__,__LINE__);L_BREAKPOINT;_exit(-1);}while(false)
#define L_ASSERT(exp) L_DEBUGONLY(if(!(exp))L_ERROR(exp is false))

#endif

