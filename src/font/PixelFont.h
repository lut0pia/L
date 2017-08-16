#pragma once

#include "Font.h"

namespace L {
  class PixelFont : public Font {
  private:
    float _ratio;
  public:
    PixelFont(int = 14);
    void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) override;
  };
}
