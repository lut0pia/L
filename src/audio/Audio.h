#pragma once

#include "../math/Vector.h"

namespace L {
  namespace Audio {
    void init();
    void listener_position(const Vector3f&);
    void listener_velocity(const Vector3f&);
    void listener_orientation(const Vector3f& forward, const Vector3f& up);
  }
}
