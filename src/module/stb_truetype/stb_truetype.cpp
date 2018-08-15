#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Font.h>

#define STB_TRUETYPE_IMPLEMENTATION 
#include "stb_truetype.h"

using namespace L;

class STBFont : public Font {
protected:
  stbtt_fontinfo _font;
  float _scale;
  int _ascent, _descent;
  Buffer _buffer;
public:
  STBFont(stbtt_fontinfo font, Buffer& buffer, uint32_t pixel_height) : _font(font), _buffer((Buffer&&)buffer) {
    stbtt_GetFontVMetrics(&_font, &_ascent, &_descent, &_lineheight);
    _scale = float(pixel_height) / float(_ascent);
    _lineheight += _ascent - _descent;
    _ascent *= _scale;
    _descent *= _scale;
    _lineheight *= _scale;
  }
  void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) override {
    int w, h;
    unsigned char* bmp = stbtt_GetCodepointBitmap(&_font, 0, _scale, utf32, &w, &h, 0, 0);
    stbtt_GetCodepointHMetrics(&_font, utf32, &out_glyph.advance, nullptr);
    stbtt_GetCodepointBitmapBox(&_font, utf32, _scale, _scale, &out_glyph.origin.x(), &out_glyph.origin.y(), nullptr, nullptr);
    out_glyph.advance *= _scale;
    out_glyph.origin.y() = _ascent + out_glyph.origin.y();
    out_bmp.resizeFast(w, h);
    for(int x(0); x < w; x++)
      for(int y(0); y < h; y++)
        out_bmp(x, y) = Color(255, 255, 255, bmp[x + w*y]);
  }
};

bool stb_truetype_loader(ResourceSlot& slot, Font*& intermediate) {
  if(Buffer buffer = slot.read_source_file()) {
    uint32_t pixel_height(16);
    if(Symbol height_param = slot.parameter("height")) {
      pixel_height = atoi(height_param);
    }
    stbtt_fontinfo font;
    if(stbtt_InitFont(&font, (uint8_t*)buffer.data(), stbtt_GetFontOffsetForIndex((uint8_t*)buffer.data(), 0))) {
      intermediate = Memory::new_type<STBFont>(font, buffer, pixel_height);
    }
    return true;
  }
  return false;
}

void stb_truetype_module_init() {
  ResourceLoading<Font>::add_loader("ttf", stb_truetype_loader);
}
