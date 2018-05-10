#include "profiling.h"

#include "../stream/CFileStream.h"
#include "../system/intrinsics.h"
#include "../system/Memory.h"

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
    events[index] = {_name, _start, duration, _fiber_id};
  }
}
ScopeMarkerFormatted::ScopeMarkerFormatted(const char* format, ...) : ScopeMarker(nullptr) {
  va_list(args);
  va_start(args, format);
  const size_t size(vsnprintf(nullptr, 0, format, args)+1);
  _name = Memory::alloc_type<char>(size);
  vsnprintf((char*)_name, size, format, args);
}
void L::flush_profiling() {
  CFileStream file_stream("trace.json", "w");
  file_stream << "{\"traceEvents\":[\n";
  bool first(true);
  for(uint32_t i(0); i<event_index; i++) {
    const ProfilingEvent& event(events[i]);
    file_stream
      << (first ? ' ' : ',')
      << "{\"ph\":\"X\",\"tid\": " << event.fiber_id << ",\"pid\":" << 0 << ",\"name\":\"" << event.name
      << "\",\"ts\":" << event.start.microseconds() << ",\"dur\":" << event.duration.microseconds() << "}\n";
    first = false;
  }
  file_stream << "]}\n";
}
