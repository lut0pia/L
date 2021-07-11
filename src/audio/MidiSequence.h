#pragma once

#include "../container/Array.h"
#include "Midi.h"
#include "../stream/serial_bin.h"
#include "../time/Time.h"

namespace L {
  struct MidiSequence {
    typedef MidiSequence Intermediate;
    struct Event {
      uint32_t delta_us;
      MidiEvent midi_event;
    };
    Array<Event> events;

    void play(uintptr_t& index, Time& time) const; // Play from index for time

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.events; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.events; }
    friend inline size_t resource_cpu_size(Intermediate& v) { return get_cpu_size(v.events); }
  };
}
