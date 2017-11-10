#pragma once

#ifdef L_DEBUG
#define L_DEBUGONLY(...) {__VA_ARGS__}
#else
#define L_DEBUGONLY(...) {}
#endif

#define L_ASSERT(exp) L_DEBUGONLY(if(!(exp))L::error("Assertion failure: %s",#exp);)
#define L_ASSERT_MSG(exp,msg) L_DEBUGONLY(if(!(exp))L::error(msg);)

namespace L {
  void error(const char* msg, ...); // Fatal, dumps callstack
  void warning(const char* msg, ...); // Non-fatal, no callstack

  // System-dependent
  void debugbreak();
  void dump_stack();
}
