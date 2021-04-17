#pragma once

#ifndef L_PROFILING
#define L_PROFILING !L_RLS
#endif

#if L_PROFILING

#include <cstdio>
#include <cstdarg>
#include "../macros.h"
#include "../time/Time.h"

#define L_SCOPE_MARKER(name) L::ScopeMarker L_CONCAT(MARKER_,__LINE__)(name)
#define L_SCOPE_MARKERF(format,...) L::ScopeMarkerFormatted L_CONCAT(MARKER_,__LINE__)(format,__VA_ARGS__)
#define L_COUNT_MARKER(name,value) count_marker(name,value)

namespace L {
  struct ProfilingEvent {
    const char* name = nullptr;
    Time start, duration;
    uint32_t fiber_id;
    size_t alloc_size = 0;
  };
  class ScopeMarker {
  protected:
    ProfilingEvent* _event;
  public:
    ScopeMarker(const char* name);
    ~ScopeMarker();
  };
  class ScopeMarkerFormatted : public ScopeMarker {
  public:
    ScopeMarkerFormatted(const char* format, ...);
  };
  void count_marker(const char* name, int64_t value);
  void flush_profiling();
}

#else
#define L_SCOPE_MARKER(name)
#define L_SCOPE_MARKERF(format,...)
#define L_COUNT_MARKER(name,value)
#endif