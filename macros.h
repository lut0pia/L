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
#define L_NoCopy(class) \
  class(const class&); \
  class& operator=(const class&);

// Allows true stringify
#define L_Stringify(n) L_Stringify_(n)
#define L_Stringify_(n) #n

// Debugging macros
#ifdef L_Debug
#define L_DebugOnly(x) x
#else
#define L_DebugOnly(x)
#endif

#define L_FileLine ("\nFile:\t" __FILE__ "\nLine:\t" L_Stringify(__LINE__))
#define L_Error(msg) throw L::Exception(L::String("\nMsg: \t")+(msg)+L::String(L_FileLine))
#define L_ErrorIf(condition,msg) if(condition) L_Error(msg)
#define L_Warning(msg) L_DebugOnly(L_Error(msg))
#define L_WarningIf(condition,msg) L_DebugOnly(L_ErrorIf(condition, msg))

#endif

