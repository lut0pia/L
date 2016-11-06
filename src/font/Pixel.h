#pragma once

#include "FontBase.h"

namespace L {
  namespace Font {
    class Pixel : public Base {
      private:
        float _ratio;
      public:
        Pixel(int=14);
        Glyph loadGlyph(uint32_t utf32);
    };
  }
}
