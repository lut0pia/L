#include <FontPacker.h>

#include <L/src/rendering/Font.h>

#include <L/src/engine/Resource.inl>

using namespace L;

static const Symbol pixel_symbol("pixel");

// Metrics:
// Line height: 11px
// Ascent: 8px
// Descent: 2px
// Size: 10px
// Cap height: 7px
// X height: 5px

bool pixel_font_loader(ResourceSlot& slot, Font::Intermediate& intermediate) {
  if(slot.ext != pixel_symbol) {
    return false;
  }

  intermediate.line_height = 11.f / 10.f;
  intermediate.ascent = 8.f / 10.f;
  FontPacker helper(intermediate);

#define O 0xff
#define _ 0x0
#define MAKE_GLYPH(utf32, width, height, ...) \
  { \
    const uint8_t bmp[]{__VA_ARGS__}; \
    Font::Glyph& glyph(intermediate.glyphs[utf32]); \
    glyph.size = {width / 10.f, height / 10.f}; \
    glyph.origin = {1.f / 10.f, 3.f / 10.f}; \
    glyph.advance = (width + 1) / 10.f; \
    helper.add_glyph(bmp, width, height, glyph); \
  }
#include "glyphs.def"
#undef MAKE_GLYPH

  ResourceLoading<Texture>::transform_internal(slot, intermediate.texture_intermediate);
  
  return true;
}

void pixel_font_module_init() {
  ResourceLoading<Font>::add_loader(pixel_font_loader);
}
