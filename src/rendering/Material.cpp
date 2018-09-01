#include "Material.h"

#include "../engine/Resource.inl"
#include "../component/Camera.h"

using namespace L;

void Material::draw(const Camera& camera, const RenderPass& render_pass, const Matrix44f& model) {
  if(Resource<Pipeline> pipeline = final_pipeline()) {
    DescriptorSet desc_set(*pipeline);
    fill_desc_set(desc_set);
    if(&render_pass==&RenderPass::light_pass()) {
      desc_set.set_descriptor("color_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
      desc_set.set_descriptor("normal_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
      desc_set.set_descriptor("depth_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(2), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    }
    const Mesh* mesh(nullptr);
    if(Resource<Mesh> mesh_res = final_mesh()) {
      mesh = &*mesh_res;
    } else if(Resource<Font> font = final_font()) {
      const String& text(final_text());
      if(text.size()>0) {
        mesh = &font->text_mesh(text).mesh;
        desc_set.set_descriptor("atlas", VkDescriptorImageInfo {Vulkan::sampler(), font->atlas(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
      }
    }
    VkCommandBuffer cmd_buffer(camera.cmd_buffer());
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline, 0, 1, &(const VkDescriptorSet&)desc_set, 0, nullptr);
    if(const Shader::Binding* constants_binding = pipeline->find_binding("Constants")) {
      vkCmdPushConstants(cmd_buffer, *pipeline, constants_binding->stage, 0, sizeof(model), &model);
    }
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
    if(mesh) {
      mesh->draw(cmd_buffer);
    } else if(uint32_t vertex_count = final_vertex_count()) {
      vkCmdDraw(cmd_buffer, vertex_count, 1, 0, 0);
    }
  }
}

bool Material::valid() const {
  const bool can_draw_without_mesh(final_vertex_count()!=0 || (final_font().is_set() && final_text().size()>0));
  return final_pipeline() && (can_draw_without_mesh || final_mesh().is_set());
}
Interval3f Material::bounds() const {
  if(Resource<Mesh> mesh = final_mesh())
    return mesh->bounds();
  else return Interval3f(Vector3f(-1.f), Vector3f(1.f));
}
Vector2f Material::gui_size() const {
  if(Resource<Font> font = final_font()) {
    const String& text = final_text();
    if(text.size()>0) {
      const Font::TextMesh& text_mesh(font->text_mesh(text));
      return text_mesh.dimensions;
    }
  } else if(Resource<Pipeline> pipeline = final_pipeline()) {
    if(&pipeline->render_pass()==&RenderPass::present_pass() &&
      _textures.size()>0 && _textures[0].value()) {
      const Texture& texture(*_textures[0].value());
      return Vector2f(texture.width(), texture.height());
    }
  }
  return Vector2f(0, 0);
}
struct L::ApplyHelper {
  Symbol applied[128];
  uintptr_t next = 0;
  bool should_apply(const Symbol& name) {
    for(uintptr_t i(0); i<next; i++)
      if(applied[i]==name)
        return false;
    applied[next++] = name;
    return true;
  }
};
void Material::fill_desc_set(DescriptorSet& desc_set, ApplyHelper* helper) {
  if(helper) {
    for(const auto& pair : _scalars) {
      if(helper->should_apply(pair.key())) {
        desc_set.set_value(pair.key(), pair.value());
      }
    }
    for(const auto& pair : _textures) {
      if(helper->should_apply(pair.key())) {
        desc_set.set_descriptor(pair.key(), VkDescriptorImageInfo {Vulkan::sampler(), pair.value() ? *pair.value() : Texture::black(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
      }
    }
    for(const auto& pair : _vectors) {
      if(helper->should_apply(pair.key())) {
        desc_set.set_value(pair.key(), pair.value());
      }
    }
    if(_parent.is_set()) {
      _parent->fill_desc_set(desc_set, helper);
    }
  } else {
    ApplyHelper local_helper;
    fill_desc_set(desc_set, &local_helper);
  }
}

template <class T> static bool set_parameter(Array<KeyValue<Symbol, T>>& array, const Symbol& name, const T& value, bool override = true) {
  for(auto& pair : array)
    if(pair.key()==name) {
      if(override) {
        pair.value() = value;
        return true;
      } else {
        return false;
      }
    }
  array.push(name, value);
  return true;
}
void Material::scalar(const Symbol& name, float scalar, bool override) {
  set_parameter(_scalars, name, scalar, override);
}
void Material::texture(const Symbol& name, const Resource<Texture>& texture, bool override) {
  set_parameter(_textures, name, texture, override);
}
void Material::vector(const Symbol& name, const Vector4f& vector, bool override) {
  set_parameter(_vectors, name, vector, override);
}
