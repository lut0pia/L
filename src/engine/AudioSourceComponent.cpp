#include "AudioSourceComponent.h"

using namespace L;

Map<Symbol, Var> AudioSourceComponent::pack() const {
  Map<Symbol, Var> data;
  data["sound_path"] = _sound_path;
  return data;
}
void AudioSourceComponent::unpack(const Map<Symbol, Var>& data) {
  String tmp;
  unpack_item(data, "sound_path", tmp);
  if(!tmp.empty()) sound(tmp);
}

void AudioSourceComponent::update() {
  _source.position(_transform->position());
}