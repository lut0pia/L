#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Font.h>

#include <FontPacker.h>

using namespace L;

static const Symbol pixel_symbol("pixel");

bool pixel_font_loader(ResourceSlot& slot, Font::Intermediate& intermediate) {
  if(slot.ext != pixel_symbol) {
    return false;
  }

  intermediate.line_height = 11.f / 9.f;
  intermediate.ascent = 7.f / 9.f;
  FontPacker helper(intermediate);

#define O 0xff
#define _ 0x0
#define MAKE_GLYPH(utf32,width,height,...) { \
  const uint8_t bmp[] {__VA_ARGS__}; \
  Font::Glyph& glyph(intermediate.glyphs[utf32]); \
  glyph.size = {width/9.f, height/9.f}; \
  glyph.origin = {0.f, 1.f/9.f}; \
  glyph.advance = (width+1)/9.f; \
  helper.add_glyph(bmp, width, height, glyph); \
}
#include "glyphs.def"
#undef MAKE_GLYPH

  return true;
}

void pixel_font_module_init() {
  ResourceLoading<Font>::add_loader(pixel_font_loader);
}
