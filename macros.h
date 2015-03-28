#ifndef DEF_L_macros
#define DEF_L_macros

#define typeof __typeof__ // static operator replaced by type name of expression

// Assign value to automatically typed variable
#define L_Auto(n,v) \
  auto n(v)

// Iterate forwards inside collection
#define L_Iter(name,it) \
  for(auto it((name).begin());it!=(name).end();++it)

// Iterate backwards inside collection
#define L_RevIter(name,it) \
  for(auto it((name).rbegin());it!=(name).rend();++it)

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
  if(1){(name).~class(); \
    new (&(name)) class parameters;}

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

