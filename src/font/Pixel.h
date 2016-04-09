#ifndef DEF_L_Font_Pixel
#define DEF_L_Font_Pixel

#include "FontBase.h"

namespace L {
  namespace Font {
    class Pixel : public Base {
      private:
        float _ratio;
      public:
        Pixel(int=14);
        Glyph loadGlyph(size_t utf32);
    };
  }
}

#endif

