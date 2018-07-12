#include "Material.h"

using namespace L;

void Material::draw(VkCommandBuffer cmd_buffer, const Matrix44f& model) {
  if(Resource<Pipeline> pipeline = final_pipeline()) {
    const DescriptorSet* desc_set(final_desc_set());
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline, 0, 1, &(const VkDescriptorSet&)*desc_set, 0, nullptr);
    vkCmdPushConstants(cmd_buffer, *pipeline, pipeline->find_binding("Constants")->stage, 0, sizeof(model), &model);
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
    if(Resource<Mesh> mesh = final_mesh()) {
      mesh->draw(cmd_buffer);
    } else {
      vkCmdDraw(cmd_buffer, final_vertex_count(), 1, 0, 0);
    }
  }
}

bool Material::valid() const {
  const bool can_draw_without_mesh(final_vertex_count()!=0);
  return final_pipeline().is_set() && (can_draw_without_mesh || final_mesh().is_set());
}
Interval3f Material::bounds() const {
  if(Resource<Mesh> mesh = final_mesh())
    return mesh->bounds();
  else return Interval3f(Vector3f(-1.f), Vector3f(1.f));
}
void Material::make_desc_set() {
  if(!_desc_set) {
    _desc_set = Memory::new_type<DescriptorSet>(*final_pipeline());

    // Recursively copy hierarchy's parameters
    Resource<Material> parent(_parent);
    while(parent.is_set()) {
      // Use override=false to avoid overriding previous values from closer parents
      for(const auto& pair : parent->_scalars) {
        scalar(pair.key(), pair.value(), false);
      }
      for(const auto& pair : parent->_textures) {
        texture(pair.key(), pair.value(), false);
      }
      for(const auto& pair : parent->_vectors) {
        vector(pair.key(), pair.value(), false);
      }
      parent = parent->_parent;
    }
  }
}

template <class T> static bool set_parameter(Array<KeyValue<Symbol, T>>& array, const Symbol& name, const T& value, bool override) {
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
  make_desc_set();
  if(set_parameter(_scalars, name, scalar, override)) {
    _desc_set->set_value(name, scalar);
  }
}
void Material::texture(const Symbol& name, const Resource<Texture>& texture, bool override) {
  make_desc_set();
  if(set_parameter(_textures, name, texture, override)) {
    // TODO: avoid force loading textures here, should only load when necessary
    _desc_set->set_descriptor(name, VkDescriptorImageInfo {Vulkan::sampler(), *texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  }
}
void Material::vector(const Symbol& name, const Vector4f& vector, bool override) {
  make_desc_set();
  if(set_parameter(_vectors, name, vector, override)) {
    _desc_set->set_value(name, vector);
  }
}
