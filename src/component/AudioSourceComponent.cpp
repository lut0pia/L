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

void AudioSourceComponent::update() {
  _source.position(_transform->position());
}