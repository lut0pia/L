#include "Material.h"

#include "../engine/Resource.inl"
#include "../component/Camera.h"
#include "DescriptorSet.h"

using namespace L;

static Table<uint32_t, Ref<Pipeline>> pipeline_cache;

template <class K, class V>
static void patch_array(Array<KeyValue<K, V>>& dst_array, const Array<KeyValue<K, V>>& src_array) {
  for(const auto& src_pair : src_array) {
    for(const auto& dst_pair : dst_array) {
      if(src_pair.key() == dst_pair.key()) {
        return;
      }
    }
    dst_array.push(src_pair);
  }
}
void Material::State::apply(const State& patch) {
  // Pipeline state
  patch_array(shaders, patch.shaders);
  if(!render_pass && patch.render_pass) {
    render_pass = patch.render_pass;
  }
  if(cull_mode == VK_CULL_MODE_BACK_BIT && patch.cull_mode != VK_CULL_MODE_BACK_BIT) {
    cull_mode = patch.cull_mode;
  }
  if(blend_mode == BlendMode::None && patch.blend_mode != BlendMode::None) {
    blend_mode = patch.blend_mode;
  }

  // Descriptor state
  patch_array(scalars, patch.scalars);
  patch_array(textures, patch.textures);
  patch_array(vectors, patch.vectors);
  if(!font.is_set() && patch.font.is_set()) {
    font = patch.font;
  }

  // Dynamic state
  if(!mesh.is_set() && patch.mesh.is_set()) {
    mesh = patch.mesh;
  }
  if(text.empty() && !patch.text.empty()) {
    text = patch.text;
  }
  if(!vertex_count && patch.vertex_count) {
    vertex_count = patch.vertex_count;
  }
}
bool Material::State::fill_desc_set(class DescriptorSet& desc_set) const {
  for(const auto& pair : scalars) {
    desc_set.set_value(pair.key(), pair.value());
  }
  for(const auto& pair : textures) {
    if(pair.value().is_loaded()) {
      desc_set.set_descriptor(pair.key(), VkDescriptorImageInfo {Vulkan::sampler(), *pair.value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    } else {
      return false;
    }
  }
  for(const auto& pair : vectors) {
    desc_set.set_value(pair.key(), pair.value());
  }

  if(font.is_set()) {
    if(font.is_loaded()) {
      desc_set.set_descriptor("atlas", VkDescriptorImageInfo {Vulkan::sampler(), font->atlas(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    } else {
      return false;
    }
  }

  return true;
}

uint32_t Material::State::pipeline_hash() const {
  uint32_t h = 0;
  hash_combine(h, shaders);
  hash_combine(h, render_pass);
  hash_combine(h, cull_mode);
  hash_combine(h, blend_mode);
  if(mesh) {
    hash_combine(h, mesh->attributes());
  }
  return h;
}
uint32_t Material::State::descriptor_hash() const {
  uint32_t h = 0;
  hash_combine(h, textures);
  hash_combine(h, vectors);
  hash_combine(h, scalars);
  hash_combine(h, font);
  return h;
}

uint32_t Material::chain_hash() const {
  // TODO: handle longer chains
  uint32_t h = 0;
  hash_combine(h, _parent);
  return h;
}

void Material::update() {
  bool update_state = false;

  if(_state_dirty) {
    update_state = true;
    _state_dirty = false;
    _last_chain_hash = chain_hash();
  } else {
    const uint32_t chain_h = chain_hash();
    if(chain_h != _last_chain_hash) {
      _last_chain_hash = chain_h;
      update_state = true;
    }
  }

  if(update_state) {
    _final_state = Material::State();

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
  }

  const uint32_t pipeline_h = _final_state.pipeline_hash();
  if(pipeline_h != _last_pipeline_hash) {
    _last_pipeline_hash = pipeline_h;
    _pipeline = nullptr;

    if(Ref<Pipeline>* pipeline = pipeline_cache.find(pipeline_h)) {
      _pipeline = *pipeline;
      return;
    }

    // Clear all existing desc sets as they're no longer valid
    for(const auto& pairing : _desc_set_pairings) {
      Memory::delete_type(pairing.desc_set);
    }
    _desc_set_pairings.clear();

    // Create pipeline
    Pipeline::Intermediate pip_int;

    if(_final_state.shaders.empty()) {
      return; // Abort because there aren't any shaders
    }
    for(const auto& shader : _final_state.shaders) {
      if(shader.value().is_loaded()) {
        pip_int.shaders.push(shader.value());
      } else {
        return; // Abort because some shaders aren't loaded yet
      }
    }
    if(_final_state.mesh) {
      pip_int.vertex_attributes = _final_state.mesh->attributes();
    }
    pip_int.blend_override = _final_state.blend_mode;
    pip_int.cull_mode = _final_state.cull_mode;
    pip_int.render_pass = _final_state.render_pass;
    pipeline_cache[pipeline_h] = _pipeline = ref<Pipeline>(pip_int);
  }
}
bool Material::valid_for_render_pass(const class RenderPass& render_pass) const {
  return _pipeline && &_pipeline->render_pass() == &render_pass;
}
void Material::draw(const Camera& camera, const RenderPass& render_pass, const Matrix44f& model) {
  if(!_pipeline) {
    return;
  }

  L_ASSERT(&_pipeline->render_pass() == &render_pass);
  const DescriptorSet* desc_set(descriptor_set(camera, *_pipeline));

  if(!desc_set) {
    return;
  }

  const Mesh* mesh(nullptr);
  if(Resource<Mesh> mesh_res = _final_state.mesh) {
    mesh = &*mesh_res;
  } else if(Resource<Font> font = _final_state.font) {
    const String& text(_final_state.text);
    if(text.size() > 0) {
      mesh = &font->text_mesh(text).mesh;
    }
  }
  VkCommandBuffer cmd_buffer(camera.cmd_buffer());
  vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline, 0, 1, &desc_set->set(), 0, nullptr);
  if(const Shader::Binding* constants_binding = _pipeline->find_binding("Constants")) {
    vkCmdPushConstants(cmd_buffer, *_pipeline, constants_binding->stage, 0, sizeof(model), &model);
  }
  vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline);
  if(mesh) {
    mesh->draw(cmd_buffer);
  } else if(uint32_t vertex_count = _final_state.vertex_count) {
    vkCmdDraw(cmd_buffer, vertex_count, 1, 0, 0);
  }
}

Interval3f Material::bounds() const {
  if(_final_state.mesh)
    return _final_state.mesh->bounds();
  else return Interval3f(Vector3f(-1.f), Vector3f(1.f));
}
Vector2f Material::gui_size() const {
  if(Resource<Font> font = _final_state.font) {
    if(_final_state.text.size() > 0) {
      const Font::TextMesh& text_mesh(font->text_mesh(_final_state.text));
      return text_mesh.dimensions;
    }
  } else if(_pipeline) {
    if(&_pipeline->render_pass() == &RenderPass::present_pass() &&
      _final_state.textures.size() > 0 && _final_state.textures[0].value()) {
      const Texture& texture(*_final_state.textures[0].value());
      return Vector2f(float(texture.width()), float(texture.height()));
    }
  }
  return Vector2f(0.f, 0.f);
}
const DescriptorSet* Material::descriptor_set(const Camera& camera, const Pipeline& pipeline) {
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
    working_pairing->valid = _final_state.fill_desc_set(*working_pairing->desc_set);
  } else {
    // Check hash of the final descriptor state
    const uint32_t descriptor_h = _final_state.descriptor_hash();

    if(_last_descriptor_hash != descriptor_h) {
      _last_descriptor_hash = descriptor_h;
      // Update all descriptor sets
      for(const DescSetPairing& pairing : _desc_set_pairings) {
        working_pairing->valid = _final_state.fill_desc_set(*pairing.desc_set);
      }
    }
  }

  if(!working_pairing->valid) {
    return nullptr; // Desc set is invalid if some resources are unloaded
  }

  if(&pipeline.render_pass() == &RenderPass::light_pass()
    && working_pairing->last_framebuffer_update < camera.framebuffer_mtime()) {
    working_pairing->desc_set->set_descriptor("color_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->desc_set->set_descriptor("normal_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->desc_set->set_descriptor("depth_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(2), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->last_framebuffer_update = camera.framebuffer_mtime();
  } else if(&pipeline.render_pass() == &RenderPass::present_pass()
    && working_pairing->last_framebuffer_update < camera.framebuffer_mtime()) {
    working_pairing->desc_set->set_descriptor("light_buffer", VkDescriptorImageInfo {Vulkan::sampler(), camera.light_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->last_framebuffer_update = camera.framebuffer_mtime();
  }
  L_ASSERT(pipeline.desc_set_layout() == working_pairing->desc_set->layout());
  return working_pairing->desc_set;
}
void Material::mark_state_dirty() {
  _state_dirty = true;
}

template <class K, class V> static bool set_parameter(Array<KeyValue<K, V>>& array, const K& name, const V& value, bool override = true) {
  for(auto& pair : array)
    if(pair.key() == name) {
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
void Material::shader(VkShaderStageFlags stage, const Resource<Shader>& shader, bool override) {
  set_parameter(_partial_state.shaders, stage, shader, override);
  mark_state_dirty();
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

static Symbol frag_symbol("frag"), fragment_symbol("fragment"), vert_symbol("vert"), vertex_symbol("vertex");
static VkShaderStageFlags symbol_to_stage(const Symbol& sym) {
  if(sym == frag_symbol || sym == fragment_symbol) {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  } else if(sym == vert_symbol || sym == vertex_symbol) {
    return VK_SHADER_STAGE_VERTEX_BIT;
  } else {
    return VK_SHADER_STAGE_ALL;
  }
}

static Symbol back_symbol("back"), front_symbol("front");
static VkCullModeFlags symbol_to_cull_mode(const Symbol& sym) {
  if(sym == back_symbol) {
    return VK_CULL_MODE_BACK_BIT;
  } else if(sym == front_symbol) {
    return VK_CULL_MODE_FRONT_BIT;
  } else {
    return VK_CULL_MODE_NONE;
  }
}

static Symbol mult_symbol("mult");
static BlendMode symbol_to_blend_mode(const Symbol& sym) {
  if(sym == mult_symbol) {
    return BlendMode::Mult;
  } else {
    return BlendMode::None;
  }
}

void Material::script_registration() {
#define L_FUNCTION(name,...) ScriptContext::global(Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {__VA_ARGS__})
#define L_METHOD(type,name,n,...) ScriptContext::type_value(Type<type*>::description(),Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {L_ASSERT(c.param_count()>=n && c.current_self().is<type*>());c.current_self().as<type*>()->__VA_ARGS__;})
#define L_RETURN_METHOD(type,name,n,...) ScriptContext::type_value(Type<type*>::description(),Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {L_ASSERT(c.param_count()>=n && c.current_self().is<type*>());c.return_value() = c.current_self().as<type*>()->__VA_ARGS__;})
  L_METHOD(Material, "parent", 1, parent(c.param(0).get<String>()));
  // Pipeline state
  L_METHOD(Material, "shader", 2, shader(symbol_to_stage(c.param(0)), c.param(1).get<String>()));
  L_METHOD(Material, "render_pass", 1, render_pass(c.param(0)));
  L_METHOD(Material, "cull_mode", 1, cull_mode(symbol_to_cull_mode(c.param(0))));
  L_METHOD(Material, "blend_mode", 1, blend_mode(symbol_to_blend_mode(c.param(0))));
  // Descriptor state
  L_METHOD(Material, "texture", 2, texture(c.param(0), c.param(1).get<String>()));
  L_METHOD(Material, "vector", 2, vector(c.param(0), c.param(1)));
  L_METHOD(Material, "scalar", 2, scalar(c.param(0), c.param(1)));
  L_METHOD(Material, "color", 2, color(c.param(0), c.param(1).get<Color>()));
  L_METHOD(Material, "font", 1, font(c.param(0).get<String>()));
  // Dynamic state
  L_METHOD(Material, "mesh", 1, mesh(c.param(0).get<String>()));
  L_METHOD(Material, "text", 1, text(c.param(0).get<String>()));
  L_METHOD(Material, "vertex_count", 1, vertex_count(uint32_t(c.param(0).get<float>())));
}