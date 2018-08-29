#include "L/src/rendering/Font.h"
#include "L/src/engine/Resource.inl"

using namespace L;

static inline void make_pixel_glyph(Font::Intermediate& intermediate, uint32_t& x, uint32_t utf32, uint32_t width, uint32_t height, const uint8_t* bitmap) {
  const uint32_t atlas_width(intermediate.texture_intermediate.width);
  const uint32_t atlas_height(intermediate.texture_intermediate.height);
  for(uint32_t row(0); row<height; row++) {
    const uintptr_t atlas_row_offset(atlas_width*row);
    const uintptr_t glyph_row_offset(width*row);
    memcpy((uint8_t*)intermediate.texture_intermediate.binary.data()+atlas_row_offset+x, bitmap+glyph_row_offset, width);
  }

  Font::Glyph glyph;
  glyph.atlas_coords = Vector2f(float(x)/atlas_width, 0.f);
  glyph.atlas_coords.add(Vector2f(float(x+width)/atlas_width, float(height)/atlas_height));
  glyph.size = {int(width), int(height)};
  glyph.origin = {0, 1};
  glyph.advance = width+1;
  intermediate.glyphs[utf32] = glyph;

  x += width;
}

bool pixel_font_loader(ResourceSlot& slot, Font::Intermediate& intermediate) {
  uint32_t total_width(0), max_height(0);

#define MAKE_GLYPH(c,w,h,...) \
  max_height = max<uint32_t>(h, max_height); \
  total_width += w;
#include "glyphs.def"
#undef MAKE_GLYPH

  intermediate.texture_intermediate.width = total_width;
  intermediate.texture_intermediate.height = max_height;
  intermediate.texture_intermediate.format = VK_FORMAT_R8_UNORM;
  intermediate.texture_intermediate.binary = Buffer(total_width*max_height);
  intermediate.line_height = 11;

  uint32_t x(0);
#define O 0xff
#define _ 0x0
#define MAKE_GLYPH(utf32,width,height,...) { \
  const uint8_t bmp[] {__VA_ARGS__}; \
  make_pixel_glyph(intermediate, x, utf32, width, height, bmp); \
}
#include "glyphs.def"
#undef MAKE_GLYPH

  slot.persistent = true;
  return true;
}

void pixel_font_module_init() {
  ResourceLoading<Font>::add_loader("pixel", pixel_font_loader);
}
