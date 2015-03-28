#include "Bitmap.h"

using namespace L;
using namespace Font;

#include "../Exception.h"

Font::Bitmap::Bitmap(const Image::Bitmap& bmp){
    size_t advance = bmp.width()/16;
    lh = a = bmp.height()/8;
    d = 0;

    for(size_t c(0);c<128;c++){
        L::Font::Glyph glyph;
        glyph.advance = advance;
        glyph.origin.x() = 0;
        glyph.origin.y() = -(int)a;
        glyph.bmp = bmp.sub(advance*(c%16),lh*(c/16),advance,lh);
        glyphs[c] = glyph;
    }
}
size_t Font::Bitmap::lineHeight(){
    return lh;
}
size_t Font::Bitmap::ascender(){
    return a;
}
size_t Font::Bitmap::descender(){
    return d;
}
Glyph Font::Bitmap::loadGlyph(size_t utf32){
    throw Exception("A bitmap font does not know that character.");
}
