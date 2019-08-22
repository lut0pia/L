#pragma once

#ifndef L_PROFILING
#ifdef L_DEBUG
#define L_PROFILING 1
#else
#define L_PROFILING 0
#endif
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
  class ScopeMarker {
  protected:
    const char* _name;
    const Time _start;
  public:
    inline ScopeMarker(const char* name) : _name(name), _start(Time::now()) {}
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