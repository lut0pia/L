#include "Base.h"

#include "../bytes/encoding.h"

using namespace L;
using namespace Font;

Glyph& Base::glyph(size_t utf32) {
  if(!_glyphs.has(utf32))
    return _glyphs[utf32] = loadGlyph(utf32);
  return _glyphs[utf32];
}
L::Bitmap Base::render(const String& s) {
  Array<size_t> str(UTF8toUTF32(s));
  L::Bitmap wtr;
  size_t penx = 0,
         peny = ascender(),
         width = 2,
         height = lineHeight();
  wtr.resize(width,height);
  for(size_t i=0; i<str.size(); i++) {
    if(str[i]==1) {
      for(size_t y=height-lineHeight(); y<height; y++)
        wtr(penx,y) = Color::white;
    } else if(str[i]=='\n') {
      penx = 0;
      peny += lineHeight();
      height += lineHeight();
      while(wtr.height()<height)
        wtr.resize(wtr.width(),wtr.height()*2);
    } else {
      Glyph& g(glyph(str[i]));
      if(penx+g.advance+1 > width) {
        width = penx+g.advance+1;
        while(wtr.width()<width)
          wtr.resize(wtr.width()*2,wtr.height());
      }
      for(size_t x=0; x<g.bmp.width(); x++)
        for(size_t y=0; y<g.bmp.height(); y++)
          if(penx+g.origin.x()+x < wtr.width() && peny+g.origin.y()+y < wtr.height())
            wtr(penx+g.origin.x()+x,peny+g.origin.y()+y) += g.bmp(x,y);
      penx += g.advance;
    }
  }
  if(wtr.width() != width || wtr.height() != height)
    wtr.resize(width,height);
  return wtr;
}
