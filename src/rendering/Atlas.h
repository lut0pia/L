#pragma once

#include "../container/IntervalTree.h"
#include "../container/Set.h"
#include "Texture.h"

namespace L {
  class Atlas : protected Texture {
  protected:
    IntervalTree<2, int, bool> _parts;
    Set<int> _xs, _ys;
  public:
    Atlas(uint32_t width = 4096, uint32_t height = 4096);
    Interval2f add(uint32_t width, uint32_t height, const void* data);
    Interval2f pixelToCoords(const Interval2i&) const;
    const Texture& texture() const { return *this; }
  };
}
