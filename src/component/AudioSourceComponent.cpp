#include "AudioSourceComponent.h"

using namespace L;

Map<Symbol, Var> AudioSourceComponent::pack() const {
  Map<Symbol, Var> data;
  data["sound"] = _sound;
  return data;
}
void AudioSourceComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "sound", _sound);
}
void AudioSourceComponent::script_registration() {
  L_COMPONENT_BIND(AudioSourceComponent, "audio-source");
  L_COMPONENT_METHOD(AudioSourceComponent, "sound", 1, sound(c.local(0).get<String>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "looping", 1, looping(c.local(0).get<bool>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "gain", 1, gain(c.local(0).get<float>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "rolloff", 1, rolloff(c.local(0).get<float>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "play", 0, play());
}

void AudioSourceComponent::update() {
  _source.position(_transform->position());
}
