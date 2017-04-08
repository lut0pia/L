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
          L_ERROR("Couldn't load face with FreeType.");
        if(FT_Set_Pixel_Sizes(face, 0, pixels))
          L_ERROR("Couldn't specify char size with FreeType.");
        if(FT_Select_Charmap(face, FT_ENCODING_UNICODE))
          L_ERROR("Couldn't find the unicode charmap with FreeType.");
        slot = face->glyph;
        ftbmp = &slot->bitmap;
        _lineheight = face->size->metrics.height >> 6;
      }
      Glyph loadGlyph(uint32_t utf32) override {
        Glyph wtr;
        if(FT_Load_Char(face, utf32, FT_LOAD_RENDER))
          L_ERROR("Couldn't load char with freetype.");
        wtr.advance = slot->advance.x >> 6;
        wtr.origin.x() = slot->bitmap_left;
        wtr.origin.y() = -slot->bitmap_top;
        wtr.bmp.resize(ftbmp->width, ftbmp->rows);
        for(int x(0); x<ftbmp->width; x++)
          for(int y(0); y<ftbmp->rows; y++)
            wtr.bmp(x, y) = L::Color(255, 255, 255, *(ftbmp->buffer+x+(ftbmp->width*y)));
        return wtr;
      }
    };
  public:
    FreeType() : Interface<Font>("ttf") {
      subscribe("otf");
      if(FT_Init_FreeType(&library))
        L_ERROR("Couldn't initialize FreeType.");
    }
    Ref<Font> from(const File& file) override {
      return ref<FTFont>(library, file.path(), 16);
    }
  };
  FreeType FreeType::instance;
}
