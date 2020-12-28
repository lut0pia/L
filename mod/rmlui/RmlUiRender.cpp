#include "RmlUiRender.h"

#include <L/src/component/Camera.h>
#include <L/src/component/ComponentPool.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/geometry.h>

using namespace L;

static const Symbol rmlui_symbol = "rmlui", vert_symbol = "vert";

static Array<VertexAttribute> vertex_attributes = {
  VertexAttribute {RenderFormat::R32G32_SFloat, VertexAttributeType::Position},
  VertexAttribute {RenderFormat::R8G8B8A8_UNorm, VertexAttributeType::Color},
  VertexAttribute {RenderFormat::R32G32_SFloat, VertexAttributeType::TexCoord},
};

RmlUiRender::RmlUiRender() {
  _textures.push(); // Nothing for null handle
}
void RmlUiRender::RenderGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle resource, const Rml::Core::Vector2f& translation) {
  _material_transforms.push();
  MaterialTransform& material_transform = _material_transforms.back();
  material_transform.component = _current_component;
  Material& material = material_transform.material;

  material.shader(ShaderStage::Vertex, _shader_vert);
  material.render_pass("present");
  material.mesh(_mesh);
  if(resource) {
    if(resource < _font_offset) {
      material.texture("tex", _textures[resource]);
      material.shader(ShaderStage::Fragment, _shader_frag_tex);
    } else {
      material.font(_fonts[resource - _font_offset]);
      material.shader(ShaderStage::Fragment, _shader_frag_font);
    }
  } else {
    material.shader(ShaderStage::Fragment, _shader_frag);
  }
  if(_enable_scissor) {
    material.scissor(_scissor);
  }
  material.vertex_count(num_indices);
  material.index_offset(uint32_t(_indices.size()));
  material.cull_mode(CullMode::None);

  material_transform.transform = translation_matrix(Vector3f(translation.x, translation.y, 0.f));

  const uintptr_t vertex_offset = _vertices.size();
  _vertices.push_array(vertices, num_vertices);
  for(int i = 0; i < num_indices; i++) {
    _indices.push(uint16_t(indices[i] + vertex_offset));
  }
}
void RmlUiRender::EnableScissorRegion(bool enable) {
  _enable_scissor = enable;
}
void RmlUiRender::SetScissorRegion(int x, int y, int width, int height) {
  _scissor = Interval2i(Vector2i(x, y), Vector2i(x + width, y + height));
}
bool RmlUiRender::LoadTexture(Rml::Core::TextureHandle& texture_handle, Rml::Core::Vector2i& texture_dimensions, const Rml::Core::String& source) {
  if(source.substr(0, 5) == "font:") {
    Resource<Font> font_res = source.substr(5).c_str();
    font_res.flush();
    if(font_res.is_loaded()) {
      texture_handle = _fonts.size() + _font_offset;
      _fonts.push(font_res);
      texture_dimensions.x = font_res->atlas().width();
      texture_dimensions.y = font_res->atlas().height();
      return true;
    } else {
      return false;
    }
  } else {
    Resource<Texture> tex_res = source.c_str();
    tex_res.flush();
    if(tex_res.is_loaded()) {
      texture_handle = _textures.size();
      _textures.push(tex_res);
      texture_dimensions.x = tex_res->width();
      texture_dimensions.y = tex_res->height();
      return true;
    } else {
      return false;
    }
  }
}

void RmlUiRender::Init() {
  L_SCOPE_MARKER("RmlUiRender::Init");

  ResourceLoading<Mesh>::add_loader(
    [](ResourceSlot& slot, Mesh::Intermediate& intermediate) {
      if(slot.ext != rmlui_symbol) {
        return false;
      }

      intermediate.attributes = vertex_attributes;
      intermediate.vertices = Buffer(sizeof(Rml::Core::Vertex));

      return true;
    });


  ResourceLoading<Shader>::add_loader(
    [](ResourceSlot& slot, Shader::Intermediate& intermediate) {
      if(slot.ext != rmlui_symbol) {
        return false;
      }

      String source;

      slot.ext = slot.parameter("stage");
      if(slot.ext == vert_symbol) {
        source =
          "layout(location = 0) in vec2 vposition;\n\
          layout(location = 1) in vec4 vcolor;\n\
          layout(location = 2) in vec2 vtexcoords;\n\
          layout(location = 0) out vec4 fcolor;\n\
          layout(location = 1) out vec2 ftexcoords;\n\
          void main() {\n\
            fcolor = vcolor;\n\
            ftexcoords = vtexcoords;\n\
            vec2 position = vposition + model[3].xy;\n\
            gl_Position = vec4(position * viewport_pixel_size.zw * 2.f - 1.f, 0, 1);\n\
          }\n";
      } else if(slot.parameters.find("tex")) {
        source =
          "layout(location = 0) in vec4 fcolor; \n\
          layout(location = 1) in vec2 ftexcoords; \n\
          layout(location = 0) out vec4 ocolor;\n\
          layout(binding = 1) uniform sampler2D tex;\n\
          void main() {\n\
            ocolor = fcolor * texture(tex, ftexcoords);\n\
          }\n";
      } else if(slot.parameters.find("font")) {
        source =
          "layout(location = 0) in vec4 fcolor; \n\
          layout(location = 1) in vec2 ftexcoords; \n\
          layout(location = 0) out vec4 ocolor;\n\
          layout(binding = 1) uniform sampler2D atlas;\n\
          void main() {\n\
            ocolor = fcolor;\n\
            const float dist = texture(atlas, ftexcoords).r;\n\
            const vec2 tex_size = textureSize(atlas, 0);\n\
            const float range = dFdx(ftexcoords.x) * tex_size.x * 0.015f;\n\
            const float thickness = 0.5f;\n\
            ocolor.a = smoothstep(range, 0.f, dist - thickness + range * 0.5f); \n\
          }\n";
      } else {
        source =
          "layout(location = 0) in vec4 fcolor; \n\
          layout(location = 1) in vec2 ftexcoords; \n\
          layout(location = 0) out vec4 ocolor;\n\
          void main() {\n\
            ocolor = fcolor;\n\
          }\n";
      }

      slot.source_buffer = Buffer(source.begin(), source.size());
      return ResourceLoading<Shader>::load_internal(slot, intermediate);
    });

  _mesh = ".rmlui";
  _shader_vert = ".rmlui?stage=vert";
  _shader_frag = ".rmlui?stage=frag";
  _shader_frag_tex = ".rmlui?stage=frag&tex";
  _shader_frag_font = ".rmlui?stage=frag&font";
}

void RmlUiRender::Shutdown() {
  _material_transforms.clear();
}

void RmlUiRender::Update() {
  L_SCOPE_MARKER("RmlUiRender::Update");
  _material_transforms.clear();

  ComponentPool<RmlUiComponent>::iterate(
    [&](RmlUiComponent& component) {
      _current_component = component.handle();
      component.update();
    });

  // Upload mesh
  if(_mesh && _vertices.size() > 0) {
    Mesh& mesh_mut = ((Mesh&)*_mesh);
    mesh_mut.load(
      _vertices.size(),
      _vertices.begin(),
      _vertices.size() * sizeof(Rml::Core::Vertex),
      vertex_attributes.begin(), vertex_attributes.size(),
      _indices.begin(), _indices.size());
  }
  _vertices.clear();
  _indices.clear();

  for(MaterialTransform& material_transform : _material_transforms) {
    material_transform.material.update();
  }
}

void RmlUiRender::Render(const class Camera& camera) {
  L_SCOPE_MARKER("RmlUiRender::Render");
  for(MaterialTransform& material_transform : _material_transforms) {
    if(RmlUiComponent* component = material_transform.component) {
      if(Camera* ui_camera = component->entity()->get_component<Camera>()) {
        if(&camera == ui_camera) {
          material_transform.material.draw(camera, RenderPass::present_pass(), material_transform.transform);
        }
      }
    }
  }
}
