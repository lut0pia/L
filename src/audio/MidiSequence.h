#pragma once

#include "../container/Array.h"
#include "Midi.h"
#include "../time/Time.h"

namespace L {
  namespace Audio {
    struct MidiSequence {
      struct Event {
        uint32_t delta_us;
        MidiEvent midi_event;
      };
      Array<Event> events;

      void play(intptr_t& index, Time& time) const; // Play from index for time
    };
  }
}
