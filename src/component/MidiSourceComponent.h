#pragma once

#include "../audio/MidiSequence.h"
#include "../engine/Resource.h"
#include "Component.h"

namespace L {
  class MidiSourceComponent : public TComponent<MidiSourceComponent, 
    ComponentFlag::Update> {
  protected:
    Resource<MidiSequence> _sequence;
    uintptr_t _play_index;
    Time _play_time;
    bool _loop, _playing;
  public:
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    void update();
    void play();
    void stop();

    inline void sequence(const char* filepath) { _sequence = filepath; }
    inline void looping(bool loop) { _loop = loop; }
    inline bool playing() const { return _playing; }
  };
}
