#pragma once

#include "../audio/Audio.h"
#include "Component.h"
#include "Transform.h"

namespace L {
  class AudioListenerComponent : public Component {
    L_COMPONENT(AudioListenerComponent)
      L_COMPONENT_HAS_UPDATE(AudioListenerComponent)
  protected:
    static Vector3f _position, _right, _forward, _up, _velocity, _last_position;
    Transform* _transform;
  public:
    void update();
    void update_components();
    static void script_registration();

    inline static const Vector3f position() { return _position; }
    inline static const Vector3f right() { return _right; }
  };
}
