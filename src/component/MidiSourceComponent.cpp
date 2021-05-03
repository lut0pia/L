#include "MidiSourceComponent.h"

#include "../engine/Engine.h"
#include "../engine/Resource.inl"

using namespace L;

void MidiSourceComponent::script_registration() {
  L_COMPONENT_BIND(MidiSourceComponent, "midi_source");
  L_SCRIPT_METHOD(MidiSourceComponent, "sequence", 1, sequence(c.param(0).get<String>()));
  L_SCRIPT_METHOD(MidiSourceComponent, "looping", 1, looping(c.param(0).get<bool>()));
  L_SCRIPT_METHOD(MidiSourceComponent, "play", 0, play());
  L_SCRIPT_METHOD(MidiSourceComponent, "stop", 0, stop());
  L_SCRIPT_RETURN_METHOD(MidiSourceComponent, "is_playing", 0, playing());
}

void MidiSourceComponent::update() {
  if(_playing) {
    const MidiSequence* sequence = _sequence.try_load();
    if(sequence == nullptr) {
      return;
    }

    _play_time += Engine::delta_time();
    sequence->play(_play_index, _play_time);
    if(_loop && _play_index >= sequence->events.size()) {
      _play_index = 0;
    }
  }
}
void MidiSourceComponent::play() {
  _play_index = 0;
  _play_time = 0;
  _playing = true;
}
void MidiSourceComponent::stop() {
  _playing = false;
  Midi::stop_all();
}
