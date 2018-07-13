#include <L/src/engine/Resource.h>
#include <L/src/font/Font.h>
#define STB_TRUETYPE_IMPLEMENTATION 
#include <stb/stb_truetype.h>

using namespace L;

class STBFont : public Font {
protected:
  stbtt_fontinfo _font;
  float _scale;
  int _ascent, _descent;
public:
  STBFont(const uint8_t* data, uint32_t pixels) {
    if(!stbtt_InitFont(&_font, data, stbtt_GetFontOffsetForIndex(data, 0)))
      error("stbtt_InitFont failed");
    stbtt_GetFontVMetrics(&_font, &_ascent, &_descent, &_lineheight);
    _scale = float(pixels) / float(_ascent);
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

void stb_truetype_loader(Resource<Font>::Slot& slot) {
  Buffer buffer(slot.read_source_file());
  slot.value = ref<STBFont>((uint8_t*)buffer.data(), 16);
}

void stb_truetype_module_init() {
  Resource<Font>::add_loader("ttf", stb_truetype_loader);
}
