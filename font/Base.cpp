#include "Base.h"

#include "../bytes/encoding.h"

using namespace L;
using namespace Font;

Glyph& Base::gGlyph(size_t utf32){
    if(glyphs.find(utf32)==glyphs.end())
        glyphs[utf32] = loadGlyph(utf32);
    return glyphs[utf32];
}
Image::Bitmap Base::render(const String& s){
    Vector<size_t> str(UTF8toUTF32(s));
    Image::Bitmap wtr;
    size_t penx = 0,
           peny = ascender(),
           width = 2,
           height = lineHeight();
    wtr.resize(width,height);

    for(size_t i=0;i<str.size();i++){
        if(str[i]==1){
            for(size_t y=height-lineHeight();y<height;y++)
                wtr(penx,y) = Color::white;
        }
        else if(str[i]=='\n'){
            penx = 0;
            peny += lineHeight();
            height += lineHeight();
            while(wtr.height()<height)
                wtr.resize(wtr.width(),wtr.height()*2);
        }
        else{
            Glyph& glyph(gGlyph(str[i]));
            if(penx+glyph.advance+1 > width){
                width = penx+glyph.advance+1;
                while(wtr.width()<width)
                    wtr.resize(wtr.width()*2,wtr.height());
            }
            for(size_t x=0;x<glyph.bmp.width();x++)
                for(size_t y=0;y<glyph.bmp.height();y++)
                    if(penx+glyph.origin.x()+x < wtr.width() && peny+glyph.origin.y()+y < wtr.height())
                        wtr(penx+glyph.origin.x()+x,peny+glyph.origin.y()+y) += glyph.bmp(x,y);
            penx += glyph.advance;
        }
    }
    if(wtr.width() != width || wtr.height() != height)
        wtr.resize(width,height);
    return wtr;
}
