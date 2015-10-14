#ifndef DEF_L_Interface_freetype
#define DEF_L_Interface_freetype

/*
  Windows link : freetype
  http://gnuwin32.sourceforge.net/packages/freetype.htm
*/

#include <L/L.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class FTFont : public L::Font::Base {
  private:
    FT_Face face;
    FT_GlyphSlot slot;
    FT_Bitmap* ftbmp;
  public:
    FTFont(const L::String& filePath, size_t pixels) {
      static FT_Library library;
      static bool FTinit(false);
      if(!FTinit) {
        if(FT_Init_FreeType(&library))
          throw L::Exception("Couldn't initialize FreeType.");
        FTinit = true;
      }
      if(FT_New_Face(library,filePath,0,&face))
        throw L::Exception("Couldn't load face with FreeType.");
      if(FT_Set_Pixel_Sizes(face,0,pixels))
        throw L::Exception("Couldn't specify char size with FreeType.");
      if(FT_Select_Charmap(face,FT_ENCODING_UNICODE))
        throw L::Exception("Couldn't find the unicode charmap with FreeType.");
      slot = face->glyph;
      ftbmp = &slot->bitmap;
    }
    size_t lineHeight() {
      return face->size->metrics.height >> 6;
    }
    size_t ascender() {
      return face->size->metrics.ascender >> 6;
    }
    size_t descender() {
      return face->size->metrics.descender >> 6;
    }
    L::Font::Glyph loadGlyph(size_t utf32) {
      L::Font::Glyph wtr;
      if(FT_Load_Char(face,utf32,FT_LOAD_RENDER))
        throw L::Exception("Couldn't load char with freetype.");
      wtr.advance = slot->advance.x >> 6;
      wtr.origin.x() = slot->bitmap_left;
      wtr.origin.y() = -slot->bitmap_top;
      wtr.bmp.resize(ftbmp->width,ftbmp->rows);
      for(int x(0); x<ftbmp->width; x++)
        for(int y(0); y<ftbmp->rows; y++)
          wtr.bmp(x,y) = L::Color(255,255,255,*(ftbmp->buffer+x+(ftbmp->width*y)));
      return wtr;
    }
};

#endif
