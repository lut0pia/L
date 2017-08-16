#pragma once

#include "FontLoader.h"

namespace L {
  class PixelFont : public FontLoader {
  protected:
    float _ratio;
    int _line_height;
  public:
    PixelFont(int = 14);
    void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) override;
    inline int line_height() override { return _line_height; }
  };
}
