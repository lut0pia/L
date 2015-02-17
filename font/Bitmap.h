#ifndef DEF_L_Font_Bitmap
#define DEF_L_Font_Bitmap

#include "Base.h"

namespace L{
    namespace Font{
        class Bitmap : public Base{
            private:
                size_t lh,a,d;
            public:
                Bitmap(const Image::Bitmap&);
                size_t lineHeight();
                size_t ascender();
                size_t descender();
                Glyph loadGlyph(size_t utf32);
        };
    }
}

#endif

