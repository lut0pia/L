#include "MidiSourceComponent.h"

#include "../engine/Engine.h"
#include "../engine/Resource.inl"

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
void MidiSourceComponent::script_registration() {
  L_COMPONENT_BIND(MidiSourceComponent, "midi-source");
  L_COMPONENT_METHOD(MidiSourceComponent, "sequence", 1, sequence(c.param(0).get<String>()));
  L_COMPONENT_METHOD(MidiSourceComponent, "looping", 1, looping(c.param(0).get<bool>()));
  L_COMPONENT_METHOD(MidiSourceComponent, "play", 0, play());
  L_COMPONENT_METHOD(MidiSourceComponent, "stop", 0, stop());
  L_COMPONENT_RETURN_METHOD(MidiSourceComponent, "is-playing", 0, playing());
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
