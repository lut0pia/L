#pragma once

#ifdef L_DEBUG
#define L_DEBUGONLY(...) {__VA_ARGS__}
#else
#define L_DEBUGONLY(...) {}
#endif

#define L_ASSERT(...) L_DEBUGONLY(if(!(__VA_ARGS__))L::error("Assertion failure: %s",#__VA_ARGS__);)
#define L_ASSERT_MSG(exp,msg) L_DEBUGONLY(if(!(exp))L::error(msg);)

namespace L {
  void error(const char* msg, ...); // Fatal, dumps callstack
  void warning(const char* msg, ...); // Non-fatal, no callstack

  // System-dependent
  void debugbreak();
  void dump_stack();
}
