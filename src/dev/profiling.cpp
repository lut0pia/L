#include "profiling.h"

#if L_PROFILING

#include "../parallelism/TaskSystem.h"
#include "../stream/CFileStream.h"
#include "../system/intrinsics.h"
#include "../system/Memory.h"
#include "../text/String.h"

using namespace L;

struct ProfilingEvent {
  const char* name;
  Time start, duration;
  uint32_t fiber_id;
};
ProfilingEvent events[2<<17];
uint32_t event_index(0);

ScopeMarker::~ScopeMarker() {
  const Time duration(Time::now()-_start);
  if(duration>0) {
    uint32_t index;
    do {
      index = event_index;
      if(index>=L_COUNT_OF(events)) {
        return;
      }
    } while(cas(&event_index, index, index+1)!=index);
    events[index] = {_name, _start, duration, TaskSystem::fiber_id()};
  }
}
ScopeMarkerFormatted::ScopeMarkerFormatted(const char* format, ...) : ScopeMarker(nullptr) {
  va_list(args);
  va_start(args, format);
  const size_t size(vsnprintf(nullptr, 0, format, args)+1);
  _name = Memory::alloc_type<char>(size);
  va_start(args, format);
  vsnprintf((char*)_name, size, format, args);
}

struct ProfilingCounterEvent {
  const char* name;
  Time time;
  int64_t value;
};
ProfilingCounterEvent counter_events[2<<17];
uint32_t counter_event_index(0);
void L::count_marker(const char* name, int64_t value) {
  uint32_t index;
  do {
    index = counter_event_index;
    if(index>=L_COUNT_OF(events)) {
      return;
    }
  } while(cas(&counter_event_index, index, index+1)!=index);
  counter_events[index] = {name, Time::now(), value};
}

void L::flush_profiling() {
  CFileStream file_stream("trace.json", "w");
  file_stream << "{\"traceEvents\":[\n";
  bool first(true);
  for(uint32_t i(0); i<event_index; i++) {
    const ProfilingEvent& event(events[i]);
    const String escaped_event_name(String(event.name) // Need some escaping to put into json
                                    .replaceAll("\n", "\\n")
                                    .replaceAll("\"", "\\\""));
    file_stream
      << (first ? ' ' : ',')
      << "{\"ph\":\"X\",\"tid\": " << event.fiber_id << ",\"pid\":" << 0 << ",\"name\":\"" << escaped_event_name
      << "\",\"ts\":" << event.start.microseconds() << ",\"dur\":" << event.duration.microseconds() << "}\n";
    first = false;
  }

  for(uint32_t i(0); i<counter_event_index; i++) {
    const ProfilingCounterEvent& event(counter_events[i]);
    const String escaped_event_name(String(event.name) // Need some escaping to put into json
      .replaceAll("\n", "\\n")
      .replaceAll("\"", "\\\""));
    if(event.time.microseconds()>0) {
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