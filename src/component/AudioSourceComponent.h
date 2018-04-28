#pragma once

#include "../audio/AudioBuffer.h"
#include "Component.h"
#include "../engine/Resource.h"

namespace L {
  class ScriptComponent;
  class Transform;
  class AudioSourceComponent : public Component {
    L_COMPONENT(AudioSourceComponent)
  protected:
    Transform* _transform;
    ScriptComponent* _script;
    Resource<AudioStream> _stream;
    float _volume;
    uint32_t _current_frame;
    bool _playing, _looping;
  public:
    inline AudioSourceComponent() : _volume(1.f), _playing(false), _looping(false) {}
    void render(void* buffer, uint32_t frame_count);

    void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    inline void stream(const char* filepath) { _stream = Resource<AudioStream>::get(filepath); }
    inline void looping(bool should_loop) { _looping = should_loop; }
    inline void volume(float v) { _volume = v; }
    inline void play() { _current_frame = 0; _playing = true; }
  };
}
