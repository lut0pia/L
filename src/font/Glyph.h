#pragma once

#include "../math/Interval.h"
#include "../math/Vector.h"

namespace L {
  struct Glyph {
    Vector2i origin, size;
    int advance;
    Interval2f atlasCoords;
    bool init = false;
  };
}
