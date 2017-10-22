#pragma once

#include "../audio/AudioSource.h"
#include "Component.h"
#include "../engine/Resource.h"
#include "Transform.h"

namespace L {
  class AudioSourceComponent : public Component {
    L_COMPONENT(AudioSourceComponent)
      L_COMPONENT_HAS_UPDATE(AudioSourceComponent)
  protected:
    Audio::Source _source;
    Transform* _transform;
    Resource<Audio::Buffer> _sound;
  public:
    void update();

    inline void update_components() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    inline void sound(const char* filepath) { _sound = Resource<Audio::Buffer>::get(filepath); _source.stop(); _source.buffer(*_sound); }
    inline void looping(bool should_loop) { _source.looping(should_loop); }
    inline void gain(float g) { _source.gain(g); }
    inline void rolloff(float r) { _source.rolloff(r); }
    inline void play() { _source.play(); }
  };
}
