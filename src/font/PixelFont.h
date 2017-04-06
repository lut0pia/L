#pragma once

#include "Font.h"

namespace L {
  class PixelFont : public Font {
  private:
    float _ratio;
  public:
    PixelFont(int = 14);
    Glyph loadGlyph(uint32_t utf32) override;
  };
}
