#pragma once

#include <cstdint> 

namespace L {
  union MidiEvent {
    enum : uint8_t {
      NoteOff = 0x8,
      NoteOn = 0x9,
      ControllerChange = 0xb,
      ProgramChange = 0xc,
    };
    uint32_t msg32;
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
    } msg;
  };
  class Midi {
  protected:
    Midi();
    virtual void send_internal(const MidiEvent&) = 0;
  public:
    static void send(const MidiEvent&);
    static void set_instrument(uint8_t channel, uint8_t instrument);
    static void play_note(uint8_t channel, uint8_t note, uint8_t velocity = 127);
    static void stop_note(uint8_t channel, uint8_t note);
    static void stop_all(uint8_t channel);
    static void stop_all();
  };
}
