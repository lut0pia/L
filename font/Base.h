#ifndef DEF_L_Font_Base
#define DEF_L_Font_Base

#include "../containers/Map.h"
#include "../image/Bitmap.h"
#include "../geometry/Vector.h"

namespace L {
  namespace Font {
    typedef struct {
      L::Bitmap bmp;
      Vector2i origin;
      int advance;
    } Glyph;
    class Base {
      protected:
        Map<uint32,Glyph> _glyphs;
        int _lineheight;
      public:
        virtual ~Base() {}
        const Glyph& glyph(uint32 utf32);
        virtual L::Bitmap render(const char*);
        virtual Glyph loadGlyph(uint32 utf32) = 0;
    };
  }
}

#endif

