#ifndef DEF_L_macros
#define DEF_L_macros

#if defined _WIN32 || _WIN64
#define L_WINDOWS
#elif defined __unix__
#define L_UNIX
#else
#error Unknown OS
#endif

#define typeof __typeof__ // static operator replaced by type name of expression
#if defined L_WINDOWS
#define popen _popen
#define pclose _pclose
#endif

// Assign value to automatically typed variable
#define L_Auto(n,v) \
  typeof(v) n(v)

// Iterate forwards inside collection
#define L_Iter(name,it) \
  for(L_Auto(it,(name).begin());it!=(name).end();++it)

// Iterate backwards inside collection
#define L_RevIter(name,it) \
  for(L_Auto(it,(name).rbegin());it!=(name).rend();++it)

/*
#define L_NoCopy(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete;
*/

// Forbids class from being copied
#define L_NoCopy(class) \
  class(const class&); \
  class& operator=(const class&);

// Start anew with an object by constructing it again (and calling the previous object's destructor)
#define L_Reconstruct(class,name,parameters) \
  ({(name).~class(); \
    new (&(name)) class parameters;})

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


namespace L {
  typedef unsigned char byte;
  typedef unsigned short ushort;
  typedef unsigned int uint;
  typedef unsigned long ulong;
  typedef unsigned long long ullong;
}

#endif

