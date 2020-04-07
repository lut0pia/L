#pragma once

#include "../container/Table.h"
#include "../rendering/Mesh.h"
#include "../rendering/Texture.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  public:
    struct Glyph {
      Vector2f origin, size;
      Interval2f atlas_coords;
      float advance;
    };
    struct Intermediate {
      Texture::Intermediate texture_intermediate;
      Table<uint32_t, Glyph> glyphs;
      float line_height;
    };
    struct TextMesh {
      Mesh mesh;
      Vector2f dimensions;
      Time last_used;
    };

  protected:
    Texture _atlas;
    Glyph _ascii[128]; // Fast path
    Table<uint32_t, Glyph> _glyphs;
    mutable Table<uint32_t, TextMesh> _text_meshes;
    mutable Time _last_update;
    float _line_height;

  public:
    Font(const Intermediate&);
    const Glyph& glyph(uint32_t utf32) const;
    const TextMesh& text_mesh(const char*) const;

    inline const Texture& atlas() const { return _atlas; }

    friend inline void resource_write(Stream& s, const Intermediate& v) {
      resource_write(s, v.texture_intermediate);
      s <= v.glyphs <= v.line_height;
    }
    friend inline void resource_read(Stream& s, Intermediate& v) {
      resource_read(s, v.texture_intermediate);
      s >= v.glyphs >= v.line_height;
    }
  };
}
