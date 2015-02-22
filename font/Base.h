#ifndef DEF_L_Font_Base
#define DEF_L_Font_Base

#include "../image.h"
#include "../geometry/Point.h"
#include "../stl/Map.h"

namespace L{
    namespace Font{
        class Glyph{
            public:
                Image::Bitmap bmp;
                Point2i origin;
                size_t advance;
        };
        class Base{
            protected:
                Map<size_t,Glyph> glyphs;
            public:
                virtual ~Base(){}
                Glyph& gGlyph(size_t utf32);
                virtual Image::Bitmap render(const String&);

                virtual size_t lineHeight() = 0;
                virtual size_t ascender() = 0;
                virtual size_t descender() = 0;
                virtual Glyph loadGlyph(size_t utf32) = 0;
        };
    }
}

#endif

