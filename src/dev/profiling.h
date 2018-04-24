#pragma once

#include <cstdio>
#include <cstdarg>
#include "../macros.h"
#include "../parallelism/TaskSystem.h"
#include "../time/Time.h"

#ifdef L_DEBUG
#define L_SCOPE_MARKER(name) L::ScopeMarker L_CONCAT(MARKER_,__LINE__)(name)
#define L_SCOPE_MARKERF(format,...) L::ScopeMarkerFormatted L_CONCAT(MARKER_,__LINE__)(format,__VA_ARGS__)
#else
#define L_SCOPE_MARKER(name)
#define L_SCOPE_MARKERF(format,...)
#endif

namespace L {
  class ScopeMarker {
  protected:
    const char* _name;
    const Time _start;
    const uint32_t _fiber_id;
  public:
    inline ScopeMarker(const char* name) : _name(name), _start(Time::now()), _fiber_id(TaskSystem::fiber_id()) {}
    ~ScopeMarker();
  };
  class ScopeMarkerFormatted : public ScopeMarker {
  protected:
    char _name_buffer[512];
  public:
    inline ScopeMarkerFormatted(const char* format, ...) : ScopeMarker(_name_buffer) {
      va_list(args);
      va_start(args, format);
      vsnprintf(_name_buffer, sizeof(_name_buffer), format, args);
    }
  };
  void flush_profiling();
}
