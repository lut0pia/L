#include "RmlUiFontEngine.h"

using namespace L;

RmlUiFontEngine::RmlUiFontEngine() {
  _font_faces.push(); // Nothing for null handle
}

bool RmlUiFontEngine::LoadFontFace(const Rml::Core::String& /*file_name*/, bool /*fallback_face*/) {
  return false;
}

Rml::Core::FontFaceHandle RmlUiFontEngine::GetFontFaceHandle(const Rml::Core::String& /*family*/, Rml::Core::Style::FontStyle /*style*/, Rml::Core::Style::FontWeight /*weight*/, int size) {
  Rml::Core::FontFaceHandle handle = _font_faces.size();
  FontFace font_face {};
  font_face.font = ".pixel";
  if(const Font* font = font_face.font.force_load()) {
    font_face.texture = Memory::new_type<Rml::Core::Texture>();
    font_face.texture->Set("font:.pixel");
    font_face.size = size;
    font_face.line_height = int(font->get_line_height() * size);
    font_face.baseline = int(font->get_base_line() * size);
    _font_faces.push(font_face);
    return handle;
  } else {
    return 0;
  }
}

int RmlUiFontEngine::GetSize(Rml::Core::FontFaceHandle handle) { return _font_faces[handle].size; }
int RmlUiFontEngine::GetXHeight(Rml::Core::FontFaceHandle handle) { return _font_faces[handle].x_height; }
int RmlUiFontEngine::GetLineHeight(Rml::Core::FontFaceHandle handle) { return _font_faces[handle].line_height; }
int RmlUiFontEngine::GetBaseline(Rml::Core::FontFaceHandle handle) { return _font_faces[handle].baseline; }
float RmlUiFontEngine::GetUnderline(Rml::Core::FontFaceHandle handle, float& thickness) { thickness = 1.f; return _font_faces[handle].underline; }
int RmlUiFontEngine::GetStringWidth(Rml::Core::FontFaceHandle handle, const Rml::Core::String& string, Rml::Core::Character) {
  FontFace& font_face = _font_faces[handle];
  if(const Font* font = font_face.font.force_load()) {
    return int(font->get_text_width(string.c_str()) * float(font_face.size));
  } else {
    return 0;
  }
}

int RmlUiFontEngine::GenerateString(Rml::Core::FontFaceHandle handle, Rml::Core::FontEffectsHandle /*font_effects_handle*/, const Rml::Core::String& string, const Rml::Core::Vector2f& position, const Rml::Core::Colourb& colour, Rml::Core::GeometryList& geometry_list) {
  FontFace& font_face = _font_faces[handle];
  const Font* font = font_face.font.force_load();
  if(font == nullptr) {
    return 0;
  }

  const Font::TextMesh& text_mesh = font->get_text_mesh(string.c_str());

  geometry_list.push_back(Rml::Core::Geometry());
  Rml::Core::Geometry& geometry = geometry_list.back();
  geometry.SetTexture(font_face.texture);
  for(const Font::Vertex& vertex : text_mesh.vertices) {
    geometry.GetIndices().push_back(int(geometry.GetVertices().size()));
    geometry.GetVertices().push_back(Rml::Core::Vertex {
        Rml::Core::Vector2f(vertex.position.x(), vertex.position.y()) * float(font_face.size) + position,
        colour,
        Rml::Core::Vector2f(vertex.texcoord.x(), vertex.texcoord.y()),
      });
  }

  return int(text_mesh.dimensions.x() * float(font_face.size));
}
