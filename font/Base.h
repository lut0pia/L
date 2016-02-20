#ifndef DEF_L_Font_Base
#define DEF_L_Font_Base

#include "../containers/Map.h"
#include "../image/Bitmap.h"
#include "../math/Vector.h"

namespace L {
  namespace Font {
    typedef struct {
      L::Bitmap bmp;
      Vector2i origin;
      int advance;
    } Glyph;
    class Base {
      protected:
        Glyph _ascii[128];
        Map<uint32,Glyph> _glyphs;
        int _lineheight;
      public:
        virtual ~Base() {}
        const Glyph& glyph(uint32 utf32);
        virtual Bitmap render(const char*);
        virtual Vector2i guessSize(const char*);
        virtual Glyph loadGlyph(uint32 utf32) = 0;
    };
  }
}

#endif

