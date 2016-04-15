#include "FontBase.h"

#include "../bytes/encoding.h"

using namespace L;
using namespace Font;

const Glyph& Base::glyph(uint32_t utf32) {
  if(utf32<128) {
    if(_ascii[utf32].bmp.empty())
      _ascii[utf32] = loadGlyph(utf32);
    return _ascii[utf32];
  } else {
    if(!_glyphs.has(utf32))
      return _glyphs[utf32] = loadGlyph(utf32);
    return _glyphs[utf32];
  }
}
Bitmap Base::render(const char* str) {
  static Bitmap wtr;
  int penx(0), peny(0), width(2), height(_lineheight);
  int utfsize;
  wtr.resizeFast(guessSize(str));
  memset(&wtr[0],0,wtr.size()*sizeof(Color));
  while(*str) {
    uint32_t utf32(UTF8toUTF32(str,&utfsize));
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
      if(penx+g.advance > width) {
        width = penx+g.advance;
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
Vector2i Base::guessSize(const char* str) {
  Vector2i wtr(512,_lineheight);
  while(*str!='\0') {
    if(*str=='\n') wtr.y() += _lineheight;
    str++;
  }
  return wtr;
}
