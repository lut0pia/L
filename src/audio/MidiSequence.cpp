#include "MidiSequence.h"

using namespace L;

void MidiSequence::play(intptr_t& index, Time& time) const {
  while(index<events.size()) {
    const Event& e(events[index]);
    const Time delta_time(e.delta_us);
    if(time>=delta_time) {
      Midi::send(e.midi_event);
      time -= delta_time;
      index++;
    } else break;
  }
}