#pragma once

#include <L/src/container/IntervalTree.h>
#include <L/src/container/Set.h>
#include <L/src/rendering/Font.h>

//! Creates signed distance field font atlas from high def bitmaps
//! Should be thread-safe
class FontPacker {
protected:
  L::Font::Intermediate& _intermediate;
  L::IntervalTree<2, int, bool> _parts;
  L::Table<uint32_t, L::Interval2i> _hash_to_part;
  L::Set<int> _xs, _ys;

public:
  FontPacker(L::Font::Intermediate&);

  void grow();
  L::Interval2i add_bmp(const uint8_t* data, uint32_t width, uint32_t height, uint32_t hash);
  L::Interval2f pixel_to_coords(const L::Interval2i& i) const;
  void add_glyph(const uint8_t* bmp, size_t width, size_t height, L::Font::Glyph& glyph);
};
