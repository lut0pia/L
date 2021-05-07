#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Font.h>

#include <FontPacker.h>

L_PUSH_NO_WARNINGS

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb_truetype.h>

L_POP_NO_WARNINGS

using namespace L;

static const Symbol ttf_symbol("ttf");

bool stb_truetype_loader(ResourceSlot& slot, Font::Intermediate& intermediate) {
  if(slot.ext != ttf_symbol) {
    return false;
  }

  Buffer source_buffer(slot.read_source_file());

  if(!source_buffer) {
    return false; // Couldn't read source file
  }

  stbtt_fontinfo font;
  if(!stbtt_InitFont(&font, (uint8_t*)source_buffer.data(), stbtt_GetFontOffsetForIndex((uint8_t*)source_buffer.data(), 0))) {
    return false; // Couldn't make sense of source file
  }

  int ascent, descent, line_gap;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
  const int height(ascent-descent);
  const float scale(1.f/height);
  const float bmp_scale(scale*128.f);
  const float inv_bmp_scale(1.f/bmp_scale);
  intermediate.line_height = scale * (height + line_gap);
  intermediate.ascent = scale * ascent;

  FontPacker helper(intermediate);

  uint32_t range_start(0), range_end(256);
  for(uint32_t utf32(range_start); utf32<range_end; utf32++) {
    Font::Glyph& glyph(intermediate.glyphs[utf32]);
    int w(0), h(0), advance(0), ox(0), oy(0);
    unsigned char* bmp = stbtt_GetCodepointBitmap(&font, bmp_scale, bmp_scale, utf32, &w, &h, 0, 0);
    stbtt_GetCodepointHMetrics(&font, utf32, &advance, nullptr);
    stbtt_GetCodepointBitmapBox(&font, utf32, 1.f, 1.f, &ox, &oy, nullptr, nullptr);
    glyph.advance = advance*scale;
    glyph.origin = {ox*scale, oy*scale+1.f};
    glyph.size = {w*scale*inv_bmp_scale, h*scale*inv_bmp_scale};
    helper.add_glyph(bmp, w, h, glyph);
  }

  return true;
}

void stb_truetype_module_init() {
  ResourceLoading<Font>::add_loader(stb_truetype_loader);
}
