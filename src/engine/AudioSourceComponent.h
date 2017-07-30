#pragma once

#include "../audio/AudioSource.h"
#include "Component.h"
#include "Resource.h"
#include "Transform.h"

namespace L {
  class AudioSourceComponent : public Component {
    L_COMPONENT(AudioSourceComponent)
  protected:
    Audio::Source _source;
    Transform* _transform;
    String _sound_path;
  public:
    void update();

    inline void updateComponents() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    inline void sound(const char* filepath) { _sound_path = filepath; _source.buffer(*Resource::sound(filepath)); }
    inline void looping(bool should_loop) { _source.looping(should_loop); }
    inline void gain(float g) { _source.gain(g); }
    inline void rolloff(float r) { _source.rolloff(r); }
    inline void play() { _source.play(); }
  };
}
