#include "profiling.h"

#if L_PROFILING

#include <atomic>

#include "../parallelism/TaskSystem.h"
#include "../rendering/Renderer.h"
#include "../stream/CFileStream.h"
#include "../system/Memory.h"
#include "../text/String.h"

using namespace L;

static ProfilingEvent events[2 << 20]{};
static std::atomic<uintptr_t> event_index = 0;

ScopeMarker::ScopeMarker(const char* name, RenderCommandBuffer* cmd) : _cmd(cmd) {
  if(name) {
    add_event(name, 0);
  }
}
ScopeMarker::~ScopeMarker() {
  _event->duration = Time::now() - _event->start;

  if(_cmd != L_SCOPE_MARKER_NOCMD) {
    Renderer::get()->end_event(_cmd);
  }
}
void ScopeMarker::add_event(const char* name, size_t alloc_size) {
  const uintptr_t index = event_index++;
  _event = events + (index % L_COUNT_OF(events));

  if(_event->alloc_size > 0) {
    Memory::free_type<char>((char*)_event->name, _event->alloc_size);
  }

  _event->name = name;
  _event->start = Time::now();
  _event->duration = 0;
  _event->fiber_id = TaskSystem::fiber_id();
  _event->alloc_size = alloc_size;

  if(_cmd != L_SCOPE_MARKER_NOCMD) {
    Renderer::get()->begin_event(_cmd, name);
  }
}
ScopeMarkerFormatted::ScopeMarkerFormatted(const char* format, RenderCommandBuffer* cmd, ...) : ScopeMarker(nullptr, cmd) {
  va_list args;
  va_start(args, cmd);
  const size_t alloc_size = vsnprintf(nullptr, 0, format, args) + size_t(1);
  char* name = Memory::alloc_type<char>(alloc_size);
  va_start(args, cmd);
  vsnprintf(name, alloc_size, format, args);
  add_event(name, alloc_size);
}

struct ProfilingCounterEvent {
  const char* name;
  Time time;
  int64_t value;
};
ProfilingCounterEvent counter_events[2 << 20]{};
std::atomic<uintptr_t> counter_event_index = 0;
void L::count_marker(const char* name, int64_t value) {
  const uintptr_t index = counter_event_index++;
  ProfilingCounterEvent* counter_event = counter_events + (index % L_COUNT_OF(counter_events));
  *counter_event = {name, Time::now(), value};
}

void L::flush_profiling() {
  CFileStream file_stream("trace.json", "w");
  file_stream << "{\"traceEvents\":[\n";
  bool first = true;
  for(uint32_t i = 0; i < L_COUNT_OF(events); i++) {
    const ProfilingEvent& event = events[i];
    if(event.name == nullptr) {
      continue;
    }

    const String escaped_event_name(String(event.name) // Need some escaping to put into json
      .replace_all("\n", "\\n")
      .replace_all("\"", "\\\""));
    file_stream
      << (first ? ' ' : ',')
      << "{\"ph\":\"X\",\"tid\": " << event.fiber_id << ",\"pid\":" << 0 << ",\"name\":\"" << escaped_event_name
      << "\",\"ts\":" << event.start.microseconds() << ",\"dur\":" << event.duration.microseconds() << "}\n";
    first = false;
  }

  for(uint32_t i(0); i < counter_event_index; i++) {
    const ProfilingCounterEvent& event(counter_events[i]);
    const String escaped_event_name(String(event.name) // Need some escaping to put into json
      .replace_all("\n", "\\n")
      .replace_all("\"", "\\\""));
    if(event.time.microseconds() > 0) {
      file_stream
        << (first ? ' ' : ',')
        << "{\"ph\":\"C\",\"pid\":" << 0 << ",\"name\":\"" << escaped_event_name
        << "\",\"ts\":" << event.time.microseconds() << ",\"args\":{\"value\":" << event.value << "}}\n";

      first = false;
    }
  }
  file_stream << "]}\n";
}

#endif