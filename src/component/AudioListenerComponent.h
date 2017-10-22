#pragma once

#include "../audio/Audio.h"
#include "Component.h"
#include "Transform.h"

namespace L {
  class AudioListenerComponent : public Component {
    L_COMPONENT(AudioListenerComponent)
      L_COMPONENT_HAS_UPDATE(AudioListenerComponent)
  protected:
    Transform* _transform;
    Vector3f _last_position;
  public:
    void update();
    void update_components();
    static void script_registration();
  };
}
