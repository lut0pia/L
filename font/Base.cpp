#include "Base.h"

#include "../bytes/encoding.h"

using namespace L;
using namespace Font;

const Glyph& Base::glyph(uint32 utf32) {
  if(!_glyphs.has(utf32))
    return _glyphs[utf32] = loadGlyph(utf32);
  return _glyphs[utf32];
}
Bitmap Base::render(const char* str) {
  int penx(0), peny(0), width(2), height(_lineheight);
  Bitmap wtr(512,128);
  int utfsize;
  while(*str) {
    uint32 utf32(UTF8toUTF32(str,&utfsize));
    if(utf32==1) { // Cursor
      for(int y(height-_lineheight); y<height; y++)
        wtr(penx,y) = Color::white;
    } else if(utf32=='\n') { // End line
      penx = 0;
      peny += _lineheight;
      height += _lineheight;
      while(wtr.height()<height)
        wtr.resize(wtr.width(),wtr.height()*2);
    } else { // Character
      const Glyph& g(glyph(utf32));
      if(penx+g.advance+1 > width) {
        width = penx+g.advance+1;
        while(wtr.width()<width)
          wtr.resize(wtr.width()*2,wtr.height());
      }
      wtr.blit(g.bmp,penx+g.origin.x(),peny+g.origin.y());
      penx += g.advance;
    }
    str += utfsize;
  }
  if(wtr.width() != width || wtr.height() != height)
    wtr.resize(width,height);
  return wtr;
}
