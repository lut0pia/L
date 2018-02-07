#pragma once

#include "../macros.h"
#include "../parallelism/TaskSystem.h"
#include "../time/Time.h"

#ifdef L_DEBUG
#define L_SCOPE_MARKER(name) L::ScopeMarker L_CONCAT(MARKER_,__LINE__)(name)
#else
#define L_SCOPE_MARKER(name)
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
  void flush_profiling();
}
