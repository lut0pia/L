#pragma once

#include <L/src/container/Array.h>
#include <L/src/engine/Resource.h>
#include <L/src/rendering/Font.h>

#include "rmlui.h"

class RmlUiFontEngine : public Rml::Core::FontEngineInterface {
protected:
  struct FontFace {
    L::Resource<L::Font> font;
    Rml::Core::Texture* texture;
    int size, x_height, line_height, baseline;
    float underline;
  };

  L::Array<FontFace> _font_faces;

public:
  RmlUiFontEngine();
  virtual bool LoadFontFace(const Rml::Core::String& file_name, bool fallback_face) override;
  virtual Rml::Core::FontFaceHandle GetFontFaceHandle(const Rml::Core::String& family, Rml::Core::Style::FontStyle style, Rml::Core::Style::FontWeight weight, int size) override;

  virtual int GetSize(Rml::Core::FontFaceHandle handle) override;
  virtual int GetXHeight(Rml::Core::FontFaceHandle handle) override;
  virtual int GetLineHeight(Rml::Core::FontFaceHandle handle) override;
  virtual int GetBaseline(Rml::Core::FontFaceHandle handle) override;
  virtual float GetUnderline(Rml::Core::FontFaceHandle handle, float& thickness) override;
  virtual int GetStringWidth(Rml::Core::FontFaceHandle handle, const Rml::Core::String& string, Rml::Core::Character) override;
  virtual int GenerateString(Rml::Core::FontFaceHandle handle, Rml::Core::FontEffectsHandle font_effects_handle, const Rml::Core::String& string, const Rml::Core::Vector2f& position, const Rml::Core::Colourb& colour, Rml::Core::GeometryList& geometry_list) override;
};
