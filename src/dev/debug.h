#pragma once

#include <stdio.h>

#define L_ASSERT(...) { if(!(__VA_ARGS__)) L::error("Assertion failure: %s",#__VA_ARGS__); }
#define L_ASSERT_MSG(exp, msg) { if(!(exp)) L::error(msg); }

namespace L {
  void init_log_file();
  void error(const char* msg, ...); // Fatal, dumps callstack
  void warning(const char* msg, ...); // Non-fatal, no callstack
  void log(const char* msg, ...);

  // System-dependent
  void debugbreak();
  void dump_stack(FILE* stream);
}
