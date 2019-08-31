#include "Material.h"

#include "../engine/Resource.inl"
#include "../component/Camera.h"
#include "DescriptorSet.h"

using namespace L;

template <class T>
static void patch_array(Array<KeyValue<Symbol, T>>& dst_array, const Array<KeyValue<Symbol, T>>& src_array) {
  for(const auto& src_pair : src_array) {
    for(const auto& dst_pair : dst_array) {
      if(src_pair.key()==dst_pair.key()) {
        return;
      }
    }
    dst_array.push(src_pair);
  }
}
void Material::State::apply(const State& patch) {
  patch_array(scalars, patch.scalars);
  patch_array(textures, patch.textures);
  patch_array(vectors, patch.vectors);

  if(!pipeline.is_set() && patch.pipeline.is_set()) {
    pipeline = patch.pipeline;
  }

  if(!mesh.is_set() && patch.mesh.is_set()) {
    mesh = patch.mesh;
  }

  if(!font.is_set() && patch.font.is_set()) {
    font = patch.font;
  }

  if(text.empty() && !patch.text.empty()) {
    text = patch.text;
  }

  if(!vertex_count && patch.vertex_count) {
    vertex_count = patch.vertex_count;
  }
}
void Material::State::fill_desc_set(class DescriptorSet& desc_set) const {
  for(const auto& pair : scalars) {
    desc_set.set_value(pair.key(), pair.value());
  }
  for(const auto& pair : textures) {
    desc_set.set_descriptor(pair.key(), VkDescriptorImageInfo {Vulkan::sampler(), pair.value() ? *pair.value() : Texture::black(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  }
  for(const auto& pair : vectors) {
    desc_set.set_value(pair.key(), pair.value());
  }

  if(font && !text.empty()) {
    desc_set.set_descriptor("atlas", VkDescriptorImageInfo {Vulkan::sampler(), font->atlas(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  }
}
void Material::State::reset() {
  pipeline = Resource<Pipeline>();
  mesh = Resource<Mesh>();
  font = Resource<Font>();
  text.clear();
  scalars.clear();
  textures.clear();
  vectors.clear();
  vertex_count = 0;
}
uint64_t Material::State::loading_state() const {
  uint64_t state(0);
  uint64_t bit(0);
  state |= uint64_t(pipeline.is_loaded())<<bit++;
  state |= uint64_t(mesh.is_loaded())<<bit++;
  state |= uint64_t(font.is_loaded())<<bit++;
  for(const auto& pair : textures) {
    state |= uint64_t(pair.value().is_loaded())<<bit++;
  }
  return state;
}

Material::Material() {
  _last_state_dirty = Time::now();
}
void Material::update() {
  if(_state_dirty || (_parent && _last_state_update<_parent->_last_state_dirty)) {
    _last_state_update = Time::now();

    Resource<Pipeline> previous_pipeline(_final_state.pipeline);
    _final_state.reset();

    // Go up material hierarchy to apply partial states
    const Material* mat(this);
    while(true) {
      _final_state.apply(mat->_partial_state);

      if(mat->_parent) {
        mat = &*mat->_parent;
      } else {
        break; // No more parents
      }
    }

    // If the pipeline has changed, clear all existing desc sets as they're no longer valid
    if(_final_state.pipeline != previous_pipeline) {
      for(const auto& pairing : _desc_set_pairings) {
        Memory::delete_type(pairing.desc_set);
      }
      _desc_set_pairings.clear();
    }

    // This will result in filling desc sets as soon as anything from the new final state is loaded
    _loading_state = 0;

    _state_dirty = false;
  }
}
bool Material::valid_for_render_pass(const class RenderPass& render_pass) const {
  return _final_state.pipeline && &_final_state.pipeline->render_pass()==&render_pass;
}
void Material::draw(const Camera& camera, const RenderPass& render_pass, const Matrix44f& model) {
  if(Resource<Pipeline> pipeline = _final_state.pipeline) {
    L_ASSERT(&pipeline->render_pass()==&render_pass);
    const DescriptorSet& desc_set(descriptor_set(camera, *pipeline));

    const Mesh* mesh(nullptr);
    if(Resource<Mesh> mesh_res = _final_state.mesh) {
      mesh = &*mesh_res;
    } else if(Resource<Font> font = _final_state.font) {
      const String& text(_final_state.text);
      if(text.size()>0) {
        mesh = &font->text_mesh(text).mesh;
      }
    }
    VkCommandBuffer cmd_buffer(camera.cmd_buffer());
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline, 0, 1, &desc_set.set(), 0, nullptr);
    if(const Shader::Binding* constants_binding = pipeline->find_binding("Constants")) {
      vkCmdPushConstants(cmd_buffer, *pipeline, constants_binding->stage, 0, sizeof(model), &model);
    }
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
    if(mesh) {
      mesh->draw(cmd_buffer);
    } else if(uint32_t vertex_count = _final_state.vertex_count) {
      vkCmdDraw(cmd_buffer, vertex_count, 1, 0, 0);
    }
  }
}

Interval3f Material::bounds() const {
  if(_final_state.mesh)
    return _final_state.mesh->bounds();
  else return Interval3f(Vector3f(-1.f), Vector3f(1.f));
}
Vector2f Material::gui_size() const {
  if(Resource<Font> font = _final_state.font) {
    if(_final_state.text.size()>0) {
      const Font::TextMesh& text_mesh(font->text_mesh(_final_state.text));
      return text_mesh.dimensions;
    }
  } else if(Resource<Pipeline> pipeline = _final_state.pipeline) {
    if(&pipeline->render_pass()==&RenderPass::present_pass() &&
      _final_state.textures.size()>0 && _final_state.textures[0].value()) {
      const Texture& texture(*_final_state.textures[0].value());
      return Vector2f(float(texture.width()), float(texture.height()));
    }
  }
  return Vector2f(0.f, 0.f);
}
const DescriptorSet& Material::descriptor_set(const Camera& camera, const Pipeline& pipeline) {
  DescSetPairing* working_pairing(nullptr);
  for(DescSetPairing& pairing : _desc_set_pairings) {
    if(pairing.camera == &camera) {
      working_pairing = &pairing;
      break;
    }
  }
  if(!working_pairing) {
    { // Create new pairing
      DescSetPairing new_pairing;
      new_pairing.camera = &camera;
      new_pairing.desc_set = Memory::new_type<DescriptorSet>(pipeline);
      new_pairing.last_framebuffer_update = 0;
      _desc_set_pairings.push(new_pairing);
    }
    working_pairing = &_desc_set_pairings.back();
    working_pairing->desc_set->set_descriptor("Shared", camera.shared_uniform().descriptor_info());
    _final_state.fill_desc_set(*working_pairing->desc_set);
  } else {
    // Check new loading state of the final material state (so many states heh?)
    const uint64_t new_loading_state(_final_state.loading_state());

    if(new_loading_state!=_loading_state) {
      // Update all descriptor sets
      for(const DescSetPairing& pairing : _desc_set_pairings) {
        _final_state.fill_desc_set(*pairing.desc_set);
      }
      _loading_state = new_loading_state;
    }
  }
  if(&pipeline.render_pass()==&RenderPass::light_pass()
    && working_pairing->last_framebuffer_update<camera.framebuffer_mtime()) {
    working_pairing->desc_set->set_descriptor("color_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->desc_set->set_descriptor("normal_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->desc_set->set_descriptor("depth_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(2), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->last_framebuffer_update = camera.framebuffer_mtime();
  }
  L_ASSERT(pipeline.desc_set_layout()==working_pairing->desc_set->layout());
  return *working_pairing->desc_set;
}
void Material::mark_state_dirty() {
  _last_state_dirty = Time::now();
  _state_dirty = true;
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
  set_parameter(_partial_state.scalars, name, scalar, override);
  mark_state_dirty();
}
void Material::texture(const Symbol& name, const Resource<Texture>& texture, bool override) {
  set_parameter(_partial_state.textures, name, texture, override);
  mark_state_dirty();
}
void Material::vector(const Symbol& name, const Vector4f& vector, bool override) {
  set_parameter(_partial_state.vectors, name, vector, override);
  mark_state_dirty();
}
