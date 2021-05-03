#include "AudioSourceComponent.h"

#include "AudioListenerComponent.h"
#include "../engine/Resource.inl"
#include "ScriptComponent.h"

using namespace L;

void AudioSourceComponent::update_components() {
  _transform = entity()->require_component<Transform>();
  _script = entity()->get_component<ScriptComponent>();
}
void AudioSourceComponent::script_registration() {
  L_COMPONENT_BIND(AudioSourceComponent, "audio_source");
  L_SCRIPT_METHOD(AudioSourceComponent, "stream", 1, stream(c.param(0).get<String>()));
  L_SCRIPT_METHOD(AudioSourceComponent, "volume", 1, volume(c.param(0).get<float>()));
  L_SCRIPT_METHOD(AudioSourceComponent, "looping", 1, looping(c.param(0).get<bool>()));
  L_SCRIPT_METHOD(AudioSourceComponent, "play", 0, play());
  L_SCRIPT_METHOD(AudioSourceComponent, "stop", 0, stop());
  L_SCRIPT_RETURN_METHOD(AudioSourceComponent, "is_playing", 0, playing());
}

void AudioSourceComponent::audio_render(void* frames, uint32_t frame_count) {
  if(_playing) {
    const AudioStream* stream = _stream.try_load();
    if(stream == nullptr) {
      return;
    }

    // Make sure we have a decoder
    if(!_decoder && !(_decoder = AudioDecoder::make(_stream))) {
      return;
    }

    if(!_decoder->playing()) {
      if(_looping) { // Restart playing
        _decoder = AudioDecoder::make(_stream);
      } else { // Stop playing
        _playing = false;
        _decoder = nullptr;
        if(_script) { // Tell script about it
          auto e(ref<Table<Var, Var>>());
          (*e)[Symbol("type")] = Symbol("AudioStop");
          _script->event(e);
        }
        return;
      }
    }
    float volumes[2] {_volume, _volume};
    if(Audio::sample_format_channels(stream->sample_format) == 1) { // Mono sounds are spatialized
      const Vector3f position(_transform->position());
      const Vector3f direction((position - AudioListenerComponent::position()).normalize());
      volumes[0] *= 1.f - max(0.f, direction.dot(AudioListenerComponent::right()));
      volumes[1] *= 1.f - max(0.f, -direction.dot(AudioListenerComponent::right()));
    }
    _decoder->render(frames, frame_count, volumes);
  }
}
