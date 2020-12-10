#include "Font.h"

#include "../text/encoding.h"

using namespace L;

Font::Font(const Intermediate& intermediate) :
  _atlas(intermediate.texture_intermediate),
  _ascii {},
  _last_update(0),
  _line_height(intermediate.line_height),
  _base_line(intermediate.base_line) {
  for(const auto& pair : intermediate.glyphs) {
    if(pair.key() < 128) {
      _ascii[pair.key()] = pair.value();
    } else {
      _glyphs[pair.key()] = pair.value();
    }
  }
}
const Font::Glyph& Font::get_glyph(uint32_t utf32) const {
  static const Font::Glyph no_glyph {};
  const Glyph* glyph = (utf32 < 128) ? (_ascii + utf32) : _glyphs.find(utf32);
  if(!glyph) {
    glyph = &no_glyph;
  }
  return *glyph;
}
float Font::get_text_width(const char* str) const {
  const uint32_t h = hash(str);
  if(TextMesh* found = _text_meshes.find(h)) {
    found->last_used = Time::now();
    return found->dimensions.x();
  } else {
    float x = 0.f, width = 0.f;
    while(*str) {
      const uint32_t utf32 = utf8_to_utf32(str);
      if(utf32 == '\n') { // End line
        x = 0.f;
      } else { // Character
        const Glyph& g = get_glyph(utf32);
        x += g.advance;
        width = max(width, x);
      }
    }
    return width;
  }
}
const Font::TextMesh& Font::get_text_mesh(const char* str) const {
  const uint32_t h = hash(str);
  if(TextMesh* found = _text_meshes.find(h)) {
    found->last_used = Time::now();
    return *found;
  } else {
    { // Removed unused cached text meshes
      static const Time second = Time(0, 0, 1);
      const Time now = Time::now();
      if(now - _last_update > second) {
        Array<uint32_t> obsolete_text_meshes;
        obsolete_text_meshes.clear();
        for(const auto& tm : _text_meshes) {
          if(now - tm.value().last_used > second) { // Not used for a second
            obsolete_text_meshes.push(tm.key());
          }
        }
        for(uint32_t obsolete_text_mesh : obsolete_text_meshes) {
          _text_meshes.remove(obsolete_text_mesh);
        }
      }
    }
    TextMesh& text_mesh = _text_meshes[h];
    text_mesh.vertices.grow_to(strlen(str) * 6);
    text_mesh.dimensions.y() = _line_height;
    float x = 0.f, y = 0.f;
    while(*str) {
      const uint32_t utf32(utf8_to_utf32(str));
      if(utf32 == '\n') { // End line
        x = 0;
        y += _line_height;
      } else { // Character
        const Glyph& g = get_glyph(utf32);
        if(g.size.x() && g.size.y()) {
          const Vertex tl = {
            Vector2f(x + g.origin.x(), y + g.origin.y()),
            Vector2f(g.atlas_coords.min().x(), g.atlas_coords.min().y()),
          };
          const Vertex tr = {
            Vector2f(tl.position.x() + g.size.x(), tl.position.y()),
            Vector2f(g.atlas_coords.max().x(), tl.texcoord.y())
          };
          const Vertex bl = {
            Vector2f(tl.position.x(), tl.position.y() + g.size.y()),
            Vector2f(tl.texcoord.x(), g.atlas_coords.max().y())
          };
          const Vertex br = {
            Vector2f(tr.position.x(), bl.position.y()),
            Vector2f(tr.texcoord.x(), bl.texcoord.y())
          };
          text_mesh.vertices.push_multiple(tl, bl, br, tl, br, tr);
        }
        x += g.advance;
        text_mesh.dimensions.x() = max(text_mesh.dimensions.x(), x);
        text_mesh.dimensions.y() = max(text_mesh.dimensions.y(), y + 1.f);
      }
    }
    static const VertexAttribute attributes[] {
      {RenderFormat::R32G32_SFloat, VertexAttributeType::Position},
      {RenderFormat::R32G32_SFloat, VertexAttributeType::TexCoord},
    };
    text_mesh.mesh.load(text_mesh.vertices.size(), text_mesh.vertices.begin(), text_mesh.vertices.size() * sizeof(Vertex), attributes, L_COUNT_OF(attributes));
    return text_mesh;
  }
}
