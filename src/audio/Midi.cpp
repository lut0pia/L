#include "Midi.h"

using namespace L;

static Midi* instance(nullptr);

Midi::Midi() {
  instance = this;
}
void Midi::send(const MidiEvent& e) {
  if(instance) {
    instance->send_internal(e);
  }
}
void Midi::set_instrument(uint8_t channel, uint8_t instrument) {
  MidiEvent e;
  e.type = MidiEvent::ProgramChange;
  e.channel = channel;
  e.instrument = instrument;
  send(e);
}
void Midi::play_note(uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiEvent e;
  e.type = MidiEvent::NoteOn;
  e.channel = channel;
  e.note = note;
  e.velocity = velocity;
  send(e);
}
void Midi::stop_note(uint8_t channel, uint8_t note) {
  MidiEvent e;
  e.type = MidiEvent::NoteOff;
  e.channel = channel;
  e.note = note;
  send(e);
}
void Midi::stop_all(uint8_t channel) {
  for(uint32_t t(0); t<128; t++)
    stop_note(channel, t);
}
void Midi::stop_all() {
  for(uint8_t c(0); c<16; c++)
    stop_all(c);
}
