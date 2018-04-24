#pragma once

#include <cstdint> 

namespace L {
  namespace Audio {
    union MidiEvent {
      enum : uint8_t {
        NoteOff = 0x8,
        NoteOn = 0x9,
        ControllerChange = 0xb,
        ProgramChange = 0xc,
      };
      uint32_t message;
      uint8_t array[4];
      struct {
        uint8_t channel : 4;
        uint8_t type : 4;
        union {
          uint8_t note;
          uint8_t instrument;
        };
        union {
          uint8_t velocity;
          uint8_t value;
        };
      };
    };
    namespace Midi {
      void send(const MidiEvent&);
      void set_instrument(uint8_t channel, uint8_t instrument);
      void play_note(uint8_t channel, uint8_t note, uint8_t velocity = 127);
      void stop_note(uint8_t channel, uint8_t note);
      void stop_all(uint8_t channel);
      void stop_all();
    };
  }
}
