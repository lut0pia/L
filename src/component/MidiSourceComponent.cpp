#include "MidiSourceComponent.h"

using namespace L;

Map<Symbol, Var> MidiSourceComponent::pack() const {
  Map<Symbol, Var> data;
  data["sequence"] = _sequence;
  data["loop"] = _loop;
  return data;
}
void MidiSourceComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "sequence", _sequence);
  unpack_item(data, "loop", _loop);
}

void MidiSourceComponent::update() {
  if(_playing && _sequence) {
    _play_time += Engine::delta_time();
    _sequence->play(_play_index, _play_time);
    if(_loop && _play_index>=_sequence->events.size())
      _play_index = 0;
  }
}
void MidiSourceComponent::play() {
  _play_index = 0;
  _play_time = 0;
  _playing = true;
}
void MidiSourceComponent::stop() {
  _playing = false;
  Audio::Midi::stop_all();
}
