#pragma once

#include <stdio.h>
#include <stdint.h>

#define L_ASSERT(...) { if(!(__VA_ARGS__)) L::error("Assertion failure: %s",#__VA_ARGS__); }
#define L_ASSERT_MSG(exp, ...) { if(!(exp)) L::error("Assertion failure: " __VA_ARGS__); }

namespace L {
  enum class DataBreakpointType : uint8_t {
    ReadOrWritten,
    Written,
    Executed,
  };

  void init_logging();

  void error(const char* msg, ...); // Fatal, dumps callstack
  void warning(const char* msg, ...); // Non-fatal, no callstack
  void log(const char* msg, ...); // Simple log
  void verbose(const char* msg, ...); // Silent log unless -v or --verbose

  // va_list versions
  void verror(const char* msg, va_list);
  void vwarning(const char* msg, va_list);
  void vlog(const char* msg, va_list);
  void vverbose(const char* msg, va_list);

  // System-dependent
  void debugbreak();
  void dump_stack(FILE* stream);
  bool set_data_breakpoint(const void* ptr, size_t size, DataBreakpointType type = DataBreakpointType::Written);
  bool unset_data_breakpoint(const void* ptr);
}
