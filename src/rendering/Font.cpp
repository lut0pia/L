#include "Font.h"

#include "../text/encoding.h"

using namespace L;

Font::Font(const Intermediate& intermediate) : _atlas(intermediate.texture_intermediate), _last_update(0), _line_height(intermediate.line_height) {
  for(const auto& pair : intermediate.glyphs) {
    if(pair.key()<128) {
      _ascii[pair.key()] = pair.value();
    } else {
      _glyphs[pair.key()] = pair.value();
    }
  }
}
const Font::Glyph& Font::glyph(uint32_t utf32) const {
  static const Font::Glyph no_glyph {};
  const Glyph* glyph((utf32<128) ? (_ascii + utf32) : _glyphs.find(utf32));
  if(!glyph) glyph = &no_glyph;
  return *glyph;
}
const Font::TextMesh& Font::text_mesh(const char* str) const {
  const uint32_t h(hash(str));
  if(TextMesh* found = _text_meshes.find(h)) {
    found->last_used = Time::now();
    return *found;
  } else {
    { // Removed unused cached text meshes
      static const Time second(0, 0, 1);
      const Time now(Time::now());
      if(now - _last_update > second) {
        Array<uint32_t> obsolete_text_meshes;
        obsolete_text_meshes.clear();
        for(const auto& tm : _text_meshes)
          if(now - tm.value().last_used > second) // Not used for a second
            obsolete_text_meshes.push(tm.key());
        for(uint32_t obsolete_text_mesh : obsolete_text_meshes)
          _text_meshes.remove(obsolete_text_mesh);
      }
    }
    TextMesh& wtr(_text_meshes[h]);
    Array<Vector4f> buffer;
    buffer.grow_to(strlen(str)*6);
    wtr.dimensions.y() = _line_height;
    float x(0.f), y(0.f);
    while(*str) {
      const uint32_t utf32(utf8_to_utf32(str));
      if(utf32=='\n') { // End line
        x = 0;
        y += _line_height;
      } else { // Character
        const Glyph& g(glyph(utf32));
        if(g.size.x() && g.size.y()) {
          const Vector4f tl(x+g.origin.x(), y+g.origin.y(), g.atlas_coords.min().x(), g.atlas_coords.min().y());
          const Vector4f tr(tl.x()+g.size.x(), tl.y(), g.atlas_coords.max().x(), tl.w());
          const Vector4f bl(tl.x(), tl.y()+g.size.y(), tl.z(), g.atlas_coords.max().y());
          const Vector4f br(tr.x(), bl.y(), tr.z(), bl.w());
          buffer.push_multiple(tl, bl, br, tl, br, tr);
        }
        x += g.advance;
        wtr.dimensions.x() = max(wtr.dimensions.x(), x);
        wtr.dimensions.y() = max(wtr.dimensions.y(), y+1.f);
      }
    }
    {
      // Go from pixel coordinates to (-1:1) coordinates
      const float inv_width(2.f/wtr.dimensions.x()), inv_height(2.f/wtr.dimensions.y());
      for(Vector4f& vector : buffer) {
        vector.x() = vector.x()*inv_width-1.f;
        vector.y() = vector.y()*inv_height-1.f;
      }
    }
    static const VertexAttribute attributes[] {
      {VK_FORMAT_R32G32_SFLOAT, VertexAttributeType::Undefined},
      {VK_FORMAT_R32G32_SFLOAT, VertexAttributeType::Undefined},
    };
    wtr.mesh.load(buffer.size(), &buffer[0], sizeof(Vector4f)*buffer.size(), attributes, L_COUNT_OF(attributes));
    return wtr;
  }
}
