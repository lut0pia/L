#include "RmlUiFontEngine.h"

using namespace L;

RmlUiFontEngine::RmlUiFontEngine() {
  _font_faces.push(); // Nothing for null handle
}

bool RmlUiFontEngine::LoadFontFace(const Rml::String& /*file_name*/, bool /*fallback_face*/) {
  return true;
}
bool RmlUiFontEngine::LoadFontFace(const Rml::byte* /*data*/, int /*data_size*/, const Rml::String& /*family*/, Rml::Style::FontStyle /*style*/, Rml::Style::FontWeight /*weight*/, bool /*fallback_face*/) {
  return true;
}

Rml::FontFaceHandle RmlUiFontEngine::GetFontFaceHandle(const Rml::String& family, Rml::Style::FontStyle /*style*/, Rml::Style::FontWeight /*weight*/, int size) {
  Rml::FontFaceHandle handle = _font_faces.size();
  FontFace font_face {};
  if(family.empty() || family == "rmlui-debugger-font") {
    font_face.font = ".pixel";
  } else {
    font_face.font = family.c_str();
  }
  if(const Font* font = font_face.font.force_load()) {
    font_face.texture = Memory::new_type<Rml::Texture>();
    font_face.texture->Set(Rml::String("font:") + (const char*)font_face.font.slot()->id);
    font_face.size = size;
    font_face.x_height = 0;
    font_face.line_height = size;
    font_face.baseline = font_face.line_height - int(font->get_ascent() * size);
    _font_faces.push(font_face);
    return handle;
  } else {
    return 0;
  }
}

int RmlUiFontEngine::GetSize(Rml::FontFaceHandle handle) { return _font_faces[handle].size; }
int RmlUiFontEngine::GetXHeight(Rml::FontFaceHandle handle) { return _font_faces[handle].x_height; }
int RmlUiFontEngine::GetLineHeight(Rml::FontFaceHandle handle) { return _font_faces[handle].line_height; }
int RmlUiFontEngine::GetBaseline(Rml::FontFaceHandle handle) { return _font_faces[handle].baseline; }
float RmlUiFontEngine::GetUnderline(Rml::FontFaceHandle handle, float& thickness) { thickness = 1.f; return _font_faces[handle].underline; }
int RmlUiFontEngine::GetStringWidth(Rml::FontFaceHandle handle, const Rml::String& string, Rml::Character) {
  FontFace& font_face = _font_faces[handle];
  if(const Font* font = font_face.font.force_load()) {
    return int(font->get_text_width(string.c_str()) * float(font_face.size));
  } else {
    return 0;
  }
}

int RmlUiFontEngine::GenerateString(Rml::FontFaceHandle handle, Rml::FontEffectsHandle /*font_effects_handle*/, const Rml::String& string, const Rml::Vector2f& position, const Rml::Colourb& colour, Rml::GeometryList& geometry_list) {
  FontFace& font_face = _font_faces[handle];
  const Font* font = font_face.font.force_load();
  if(font == nullptr) {
    return 0;
  }

  const Font::TextMesh& text_mesh = font->get_text_mesh(string.c_str());

  geometry_list.push_back(Rml::Geometry());
  Rml::Geometry& geometry = geometry_list.back();
  geometry.SetTexture(font_face.texture);
  for(const Font::Vertex& vertex : text_mesh.vertices) {
    geometry.GetIndices().push_back(int(geometry.GetVertices().size()));
    // We use the font face's size to scale the engine mesh to the wanted size
    // since the engine mesh was made normalized to the text height
    // We also shift in y with an offset of 1 because engine mesh uses bottom-left origins
    // however RmlUi uses top-left origins
    geometry.GetVertices().push_back(Rml::Vertex {
        Rml::Vector2f(vertex.position.x(), vertex.position.y() - 1.f) * float(font_face.size) + position,
        colour,
        Rml::Vector2f(vertex.texcoord.x(), vertex.texcoord.y()),
      });
  }

  return int(text_mesh.dimensions.x() * float(font_face.size));
}
