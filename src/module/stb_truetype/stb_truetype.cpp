#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Font.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

using namespace L;

bool stb_truetype_loader(ResourceSlot& slot, Font::Intermediate& intermediate) {
  Buffer source_buffer(slot.read_source_file());

  if(!source_buffer) {
    return false; // Couldn't read source file
  }

  stbtt_fontinfo font;
  if(!stbtt_InitFont(&font, (uint8_t*)source_buffer.data(), stbtt_GetFontOffsetForIndex((uint8_t*)source_buffer.data(), 0))) {
    return false; // Couldn't make sense of source file
  }

  uint32_t height(16), range_start(0), range_size(256);
  slot.parameter("height", height);

  const uint32_t atlas_width(16*height), atlas_height(16*height);

  intermediate.texture_intermediate.format = VK_FORMAT_R8_UNORM;
  intermediate.texture_intermediate.width = atlas_width;
  intermediate.texture_intermediate.height = atlas_height;
  intermediate.texture_intermediate.binary = Buffer(atlas_width*atlas_height);

  float scale;
  int ascent, descent;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, &intermediate.line_height);
  scale = float(height) / float(ascent);
  intermediate.line_height += ascent - descent;
  ascent *= scale;
  descent *= scale;
  intermediate.line_height *= scale;

  stbtt_pack_context pack_context;
  Array<stbtt_packedchar> packed_chars;
  packed_chars.size(range_size);

  stbtt_PackBegin(&pack_context, (uint8_t*)intermediate.texture_intermediate.binary.data(), atlas_width, atlas_height, 0, 0, nullptr);
  stbtt_PackFontRange(&pack_context, (uint8_t*)source_buffer.data(), 0, height, range_start, range_size, packed_chars.begin());
  stbtt_PackEnd(&pack_context);

  uint32_t utf32(range_start);
  for(const stbtt_packedchar& packed_char : packed_chars) {
    Font::Glyph glyph;
    glyph.advance = packed_char.xadvance;
    glyph.origin = {int(packed_char.xoff), int(packed_char.yoff+ascent)};
    glyph.size = {packed_char.x1-packed_char.x0, packed_char.y1-packed_char.y0};
    glyph.atlas_coords = Vector2f(float(packed_char.x0)/atlas_width, float(packed_char.y0)/atlas_height);
    glyph.atlas_coords.add(Vector2f(float(packed_char.x1)/atlas_width, float(packed_char.y1)/atlas_height));
    intermediate.glyphs[utf32++] = glyph;
  }
  return true;
}

void stb_truetype_module_init() {
  ResourceLoading<Font>::add_loader("ttf", stb_truetype_loader);
}
