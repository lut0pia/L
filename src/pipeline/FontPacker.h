#pragma once

#include "../container/IntervalTree.h"
#include "../container/Set.h"
#include "../rendering/Font.h"

namespace L {
  //! Creates signed distance field font atlas from high def bitmaps
  //! Should be thread-safe
  class FontPacker {
  protected:
    Font::Intermediate& _intermediate;
    IntervalTree<2, int, bool> _parts;
    Table<uint32_t, Interval2i> _hash_to_part;
    Set<int> _xs, _ys;

  public:
    FontPacker(Font::Intermediate&);

    void grow();
    Interval2i add_bmp(const uint8_t* data, uint32_t width, uint32_t height, uint32_t hash);
    Interval2f pixel_to_coords(const Interval2i& i) const;
    void add_glyph(const uint8_t* bmp, size_t width, size_t height, Font::Glyph& glyph);
  };
}
