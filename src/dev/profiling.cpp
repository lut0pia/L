#include "profiling.h"

#if L_PROFILING

#include "../parallelism/TaskSystem.h"
#include "../stream/CFileStream.h"
#include "../system/intrinsics.h"
#include "../system/Memory.h"
#include "../text/String.h"

using namespace L;

static ProfilingEvent events[2 << 20];
static uint32_t event_index(0);

ScopeMarker::ScopeMarker(const char* name) {
  uint32_t index;
  do {
    index = event_index;
  } while(cas(&event_index, index, (index + 1) % L_COUNT_OF(events)) != index);
  _event = events + index;

  if(_event->alloc_size > 0) {
    Memory::free_type<char>((char*)_event->name, _event->alloc_size);
  }

  _event->name = name;
  _event->start = Time::now();
  _event->duration = 0;
  _event->fiber_id = TaskSystem::fiber_id();
  _event->alloc_size = 0;
}
ScopeMarker::~ScopeMarker() {
  _event->duration = Time::now() - _event->start;
}
ScopeMarkerFormatted::ScopeMarkerFormatted(const char* format, ...) : ScopeMarker(nullptr) {
  va_list args;
  va_start(args, format);
  _event->alloc_size = vsnprintf(nullptr, 0, format, args) + 1;
  _event->name = Memory::alloc_type<char>(_event->alloc_size);
  va_start(args, format);
  vsnprintf((char*)_event->name, _event->alloc_size, format, args);
}

struct ProfilingCounterEvent {
  const char* name;
  Time time;
  int64_t value;
};
ProfilingCounterEvent counter_events[2 << 20];
uint32_t counter_event_index(0);
void L::count_marker(const char* name, int64_t value) {
  uint32_t index;
  do {
    index = counter_event_index;
  } while(cas(&counter_event_index, index, (index + 1) % L_COUNT_OF(counter_events)) != index);
  counter_events[index] = {name, Time::now(), value};
}

void L::flush_profiling() {
  CFileStream file_stream("trace.json", "w");
  file_stream << "{\"traceEvents\":[\n";
  bool first(true);
  for(uint32_t i(0); i < event_index; i++) {
    const ProfilingEvent& event(events[i]);
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