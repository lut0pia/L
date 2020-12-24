#pragma once

#include <stdio.h>

#define L_ASSERT(...) { if(!(__VA_ARGS__)) L::error("Assertion failure: %s",#__VA_ARGS__); }
#define L_ASSERT_MSG(exp, ...) { if(!(exp)) L::error("Assertion failure: " __VA_ARGS__); }

namespace L {
  void init_log_file();

  void error(const char* msg, ...); // Fatal, dumps callstack
  void warning(const char* msg, ...); // Non-fatal, no callstack
  void log(const char* msg, ...);

  // va_list versions
  void verror(const char* msg, va_list);
  void vwarning(const char* msg, va_list);
  void vlog(const char* msg, va_list);

  // System-dependent
  void debugbreak();
  void dump_stack(FILE* stream);
}
