#pragma once

#include "../containers/Table.h"
#include "../image/Bitmap.h"
#include "../gl/Atlas.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  protected:
    struct Glyph {
      L::Bitmap bmp;
      Vector2i origin;
      int advance;
      Interval2f atlasCoords;
    };
    struct TextMesh {
      String str;
      GL::Mesh mesh;
      Vector2i dimensions = {0,0};
      Time lastUsed;
    };
    GL::Atlas _atlas;
    Glyph _ascii[128];
    Table<uint32_t, Glyph> _glyphs;
    Table<uint32_t, TextMesh> _textMeshes;
    int _lineheight;

  public:
    virtual ~Font() {}
    const Glyph& glyph(uint32_t utf32);
    virtual TextMesh& textMesh(const char*);
    virtual void draw(int x, int y, const char*, Vector2f anchor = Vector2f(0.f, 0.f));
    virtual Glyph loadGlyph(uint32_t utf32) = 0;

    inline int lineHeight() const { return _lineheight; }

    void updateTextMeshes();
  };
}
