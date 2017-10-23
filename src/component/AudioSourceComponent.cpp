#include "AudioSourceComponent.h"

#include "AudioListenerComponent.h"

using namespace L;

Map<Symbol, Var> AudioSourceComponent::pack() const {
  Map<Symbol, Var> data;
  data["sound"] = _sound;
  data["volume"] = _volume;
  data["looping"] = _looping;
  return data;
}
void AudioSourceComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "sound", _sound);
  unpack_item(data, "volume", _volume);
  unpack_item(data, "looping", _looping);
}
void AudioSourceComponent::script_registration() {
  L_COMPONENT_BIND(AudioSourceComponent, "audio-source");
  L_COMPONENT_METHOD(AudioSourceComponent, "sound", 1, sound(c.local(0).get<String>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "volume", 1, volume(c.local(0).get<float>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "looping", 1, looping(c.local(0).get<bool>()));
  L_COMPONENT_METHOD(AudioSourceComponent, "play", 0, play());
}

void AudioSourceComponent::render(void* buffer, uint32_t frame_count) {
  if(_playing && _sound) {
    if(_current_frame>=_sound->sample_count()) {
      if(_looping)
        _current_frame = 0;
      else {
        _playing = false;
        return;
      }
    }
    float volumes[2]{_volume,_volume};
    if(Audio::sample_format_channels(_sound->format())==1) { // Mono sounds are spatialized
      const Vector3f position(_transform->position());
      const Vector3f direction((position-AudioListenerComponent::position()).normalize());
      volumes[0] *= 1.f-max(0.f, direction.dot(AudioListenerComponent::right()));
      volumes[1] *= 1.f-max(0.f, -direction.dot(AudioListenerComponent::right()));
    }
    _sound->render(buffer, _current_frame, frame_count, volumes);
    _current_frame += frame_count;
  }
}
