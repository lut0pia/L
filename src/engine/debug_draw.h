#pragma once

#include "../math/Vector.h"
#include "../rendering/Color.h"
#include "../time/Time.h"

namespace L {
  void init_debug_draw();
  void debug_draw_line(const Vector3f& a, const Vector3f& b, const Color& color);
}
