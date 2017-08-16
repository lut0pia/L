#pragma once

#include "Glyph.h"
#include "../image/Bitmap.h"

namespace L {
  class FontLoader {
  public:
    virtual void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) = 0;
    virtual int line_height() = 0;
  };
}
