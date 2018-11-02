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

    void play(intptr_t& index, Time& time) const; // Play from index for time

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.events; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.events; }
  };
}
