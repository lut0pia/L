#pragma once

#include <L/src/container/Array.h>
#include <L/src/engine/Resource.h>
#include <L/src/rendering/Font.h>

#include "rmlui.h"

class RmlUiFontEngine : public Rml::FontEngineInterface {
protected:
  struct FontFace {
    L::Resource<L::Font> font;
    Rml::Texture* texture;
    int size, x_height, line_height, baseline;
    float underline;
  };

  L::Array<FontFace> _font_faces;

public:
  RmlUiFontEngine();
  virtual bool LoadFontFace(const Rml::String& file_name, bool fallback_face) override;
  virtual bool LoadFontFace(const Rml::byte* data, int data_size, const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, bool fallback_face) override;
  virtual Rml::FontFaceHandle GetFontFaceHandle(const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, int size) override;

  virtual int GetSize(Rml::FontFaceHandle handle) override;
  virtual int GetXHeight(Rml::FontFaceHandle handle) override;
  virtual int GetLineHeight(Rml::FontFaceHandle handle) override;
  virtual int GetBaseline(Rml::FontFaceHandle handle) override;
  virtual float GetUnderline(Rml::FontFaceHandle handle, float& thickness) override;
  virtual int GetStringWidth(Rml::FontFaceHandle handle, const Rml::String& string, Rml::Character) override;
  virtual int GenerateString(Rml::FontFaceHandle handle, Rml::FontEffectsHandle font_effects_handle, const Rml::String& string, const Rml::Vector2f& position, const Rml::Colourb& colour, Rml::GeometryList& geometry_list) override;
};
