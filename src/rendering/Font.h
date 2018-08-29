#pragma once

#include "../container/Table.h"
#include "../engine/Resource.h"
#include "../rendering/Mesh.h"
#include "../rendering/Pipeline.h"
#include "../rendering/Texture.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  public:
    struct Glyph {
      Vector2i origin, size;
      Interval2f atlas_coords;
      int advance;
    };
    struct Intermediate {
      Texture::Intermediate texture_intermediate;
      Table<uint32_t, Glyph> glyphs;
      int line_height;
    };

  protected:
    struct TextMesh {
      String str;
      Mesh mesh;
      Vector2i dimensions = {0,0};
      Time last_used;
    };

    static Resource<Pipeline> _pipeline;
    Texture _atlas;
    Glyph _ascii[128]; // Fast path
    Table<uint32_t, Glyph> _glyphs;
    Table<uint32_t, TextMesh> _text_meshes;
    Time _last_update;
    int _line_height;

  public:
    Font(const Intermediate&);
    const Glyph& glyph(uint32_t utf32);
    TextMesh& text_mesh(const char*);
    void draw(VkCommandBuffer cmd_buffer, const Matrix44f& model, const char*, Vector2f anchor = Vector2f(0.f, 0.f));
    void update();

    inline static void pipeline(const char* path) { _pipeline = path; }

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.texture_intermediate <= v.glyphs <= v.line_height; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.texture_intermediate >= v.glyphs >= v.line_height; }
  };
}
