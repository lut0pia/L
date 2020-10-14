#pragma once

#include "../audio/AudioDecoder.h"
#include "../audio/AudioStream.h"
#include "Component.h"
#include "../engine/Resource.h"

namespace L {
  class AudioSourceComponent : public TComponent<AudioSourceComponent, 
    ComponentFlag::AudioRender> {
  protected:
    class Transform* _transform;
    class ScriptComponent* _script;
    Resource<AudioStream> _stream;
    Ref<AudioDecoder> _decoder;
    float _volume = 1.f;
    bool _playing = false, _looping = false;

  public:
    void audio_render(void* frames, uint32_t frame_count);

    void update_components() override;
    static void script_registration();

    inline void stream(const char* filepath) { _stream = filepath; }
    inline void looping(bool should_loop) { _looping = should_loop; }
    inline void volume(float v) { _volume = v; }
    inline void play() { _decoder = nullptr; _playing = true; }
    inline void stop() { _playing = false; }
    inline bool playing() const { return _playing; }
  };
}
