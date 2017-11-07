#pragma once

/*
  Windows link : freetype
  http://gnuwin32.sourceforge.net/packages/freetype.htm
*/

#include <L/src/L.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace L {
  class FreeType : public Interface<Font> {
    static FreeType instance;
  private:
    FT_Library library;

    class FTFont : public Font {
    private:
      FT_Face face;
      FT_GlyphSlot slot;
      FT_Bitmap* ftbmp;
    public:
      FTFont(FT_Library library, const char* filePath, uint32_t pixels) {
        if(FT_New_Face(library, filePath, 0, &face))
          error("Couldn't load face with FreeType.");
        if(FT_Set_Pixel_Sizes(face, 0, pixels))
          error("Couldn't specify char size with FreeType.");
        if(FT_Select_Charmap(face, FT_ENCODING_UNICODE))
          error("Couldn't find the unicode charmap with FreeType.");
        slot = face->glyph;
        ftbmp = &slot->bitmap;
        _lineheight = face->size->metrics.height >> 6;
      }
      void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) override {
        if(FT_Load_Char(face, utf32, FT_LOAD_RENDER))
          error("Couldn't load char with freetype.");
        out_glyph.advance = slot->advance.x >> 6;
        out_glyph.origin.x() = slot->bitmap_left;
        out_glyph.origin.y() = -slot->bitmap_top;
        out_bmp.resizeFast(ftbmp->width, ftbmp->rows);
        for(int x(0); x<ftbmp->width; x++)
          for(int y(0); y<ftbmp->rows; y++)
            out_bmp(x, y) = L::Color(255, 255, 255, *(ftbmp->buffer+x+(ftbmp->width*y)));
      }
    };
  public:
    FreeType() : Interface<Font>("ttf") {
      subscribe("otf");
      if(FT_Init_FreeType(&library))
        error("Couldn't initialize FreeType.");
    }
    Ref<Font> from(const File& file) override {
      return ref<FTFont>(library, file.path(), 16);
    }
  };
  FreeType FreeType::instance;
}
