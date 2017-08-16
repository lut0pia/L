#pragma once

#include "../containers/Ref.h"
#include "../containers/Table.h"
#include "FontLoader.h"
#include "../image/Bitmap.h"
#include "../gl/Atlas.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  private:
    Bitmap _bmp;
  protected:
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
    Ref<FontLoader> _loader;
    int _line_height;

  public:
    Font(const Ref<FontLoader>&);
    const Glyph& glyph(uint32_t utf32);
    TextMesh& textMesh(const char*);
    void draw(int x, int y, const char*, Vector2f anchor = Vector2f(0.f, 0.f));

    void updateTextMeshes();
  };
}
