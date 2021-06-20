#pragma once

#include <L/src/rendering/Material.h>
#include <L/src/rendering/Mesh.h>

#include "rmlui.h"
#include "RmlUiComponent.h"

class RmlUiRender : public Rml::RenderInterface {
protected:
  struct MaterialTransform {
    L::Handle<RmlUiComponent> component;
    L::Material material;
    L::Matrix44f transform;
  };

  L::Array<Rml::Vertex> _vertices;
  L::Array<uint16_t> _indices;
  L::Resource<L::Mesh> _mesh;

  bool _enable_scissor = false;
  L::Interval2i _scissor;

  L::Handle<RmlUiComponent> _current_component;
  L::Array<MaterialTransform> _material_transforms;
  L::Array<L::Resource<L::Texture>> _textures;
  L::Array<L::Resource<L::Font>> _fonts;
  const size_t _font_offset = 1 << 24;

  L::Resource<L::Shader> _shader_vert, _shader_frag, _shader_frag_tex, _shader_frag_font;

public:
  RmlUiRender();
  virtual void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle resource, const Rml::Vector2f& translation) override;
  virtual void EnableScissorRegion(bool enable) override;
  virtual void SetScissorRegion(int x, int y, int width, int height) override;
  virtual bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

  void Init();
  void Shutdown();
  void Update();
  void Render(const class L::Camera& camera);
};
