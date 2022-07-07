#pragma once

#ifndef L_PROFILING
#define L_PROFILING L_USE_PROFILING && !L_RLS
#endif

#if L_PROFILING

#include <cstdio>
#include <cstdarg>
#include "../macros.h"
#include "../time/Time.h"

#define L_SCOPE_MARKER_NOCMD ((struct L::RenderCommandBuffer*)~uintptr_t(0))
#define L_SCOPE_MARKER(name) L::ScopeMarker L_CONCAT(MARKER_,__LINE__)(name,L_SCOPE_MARKER_NOCMD)
#define L_SCOPE_MARKERF(format,...) L::ScopeMarkerFormatted L_CONCAT(MARKER_,__LINE__)(format,L_SCOPE_MARKER_NOCMD,__VA_ARGS__)
#define L_SCOPE_GPU_MARKER(cmd,name) L::ScopeMarker L_CONCAT(MARKER_,__LINE__)(name,cmd)
#define L_SCOPE_GPU_MARKERF(cmd,format,...) L::ScopeMarkerFormatted L_CONCAT(MARKER_,__LINE__)(format,cmd,__VA_ARGS__)
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
    ProfilingEvent* _event = nullptr;
    struct RenderCommandBuffer* _cmd = nullptr;
  public:
    ScopeMarker(const char* name, struct RenderCommandBuffer*);
    ~ScopeMarker();
    void add_event(const char* name, size_t alloc_size);
  };
  class ScopeMarkerFormatted : public ScopeMarker {
  public:
    ScopeMarkerFormatted(const char* format, struct RenderCommandBuffer*, ...);
  };
  void count_marker(const char* name, int64_t value);
  void flush_profiling();
}

#else
#define L_SCOPE_MARKER(name)
#define L_SCOPE_MARKERF(format,...)
#define L_SCOPE_GPU_MARKER(cmd,name)
#define L_SCOPE_GPU_MARKERF(cmd,format,...)
#define L_COUNT_MARKER(name,value)
#endif