#pragma once

#include "../containers/Table.h"
#include "../image/Bitmap.h"
#include "../gl/Atlas.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  private:
    Bitmap _bmp;
  protected:
    struct Glyph {
      Vector2i origin, size;
      int advance;
      Interval2f atlasCoords;
      bool init = false;
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
    virtual void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) = 0;

    inline int lineHeight() const { return _lineheight; }

    void updateTextMeshes();
  };
}
