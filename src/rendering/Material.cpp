#include "Material.h"

#include "../engine/Resource.inl"
#include "../component/Camera.h"

using namespace L;

static Table<uint32_t, Ref<Pipeline>> pipeline_cache;
static const Interval2i default_scissor = Vector2i(0);

template <class K, class V>
static void patch_array(Array<KeyValue<K, V>>& dst_array, const Array<KeyValue<K, V>>& src_array) {
  for(const auto& src_pair : src_array) {
    bool already_defined = false;
    for(const auto& dst_pair : dst_array) {
      if(src_pair.key() == dst_pair.key()) {
        already_defined = true;
        break;
      }
    }
    if(!already_defined) {
      dst_array.push(src_pair);
    }
  }
}
void Material::State::apply(const State& patch) {
  // Pipeline state
  patch_array(pipeline.shaders, patch.pipeline.shaders);
  if(!pipeline.render_pass && patch.pipeline.render_pass) {
    pipeline.render_pass = patch.pipeline.render_pass;
  }
#define PATCH_VALUE(name, default_value) \
  if(pipeline.name == default_value && patch.pipeline.name != default_value){ \
    pipeline.name = patch.pipeline.name; \
  }
  PATCH_VALUE(polygon_mode, VK_POLYGON_MODE_MAX_ENUM);
  PATCH_VALUE(cull_mode, VK_CULL_MODE_FLAG_BITS_MAX_ENUM);
  PATCH_VALUE(topology, VK_PRIMITIVE_TOPOLOGY_MAX_ENUM);
  PATCH_VALUE(blend_mode, BlendMode::None);
#undef PATCH_VALUE

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
#define PATCH_VALUE(name, default_value) \
  if(name == default_value && patch.name != default_value) { \
    name = patch.name; \
  }
  PATCH_VALUE(scissor, default_scissor);
  PATCH_VALUE(vertex_count, 0);
  PATCH_VALUE(index_offset, 0);
  PATCH_VALUE(vertex_offset, 0);
  PATCH_VALUE(text, "");
#undef PATCH_VALUE
}

uint32_t Material::State::pipeline_hash() const {
  uint32_t h = 0;
  hash_combine(h, pipeline.shaders);
  hash_combine(h, pipeline.render_pass);
  hash_combine(h, pipeline.polygon_mode);
  hash_combine(h, pipeline.cull_mode);
  hash_combine(h, pipeline.topology);
  hash_combine(h, pipeline.blend_mode);
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

VkDescriptorSet Material::descriptor_set(const Camera& camera, const Pipeline& pipeline) {
  DescSetPairing* working_pairing(nullptr);
  for(DescSetPairing& pairing : _desc_set_pairings) {
    if(pairing.camera == &camera) {
      working_pairing = &pairing;
      break;
    }
  }
  if(!working_pairing) {
    { // Create new pairing
      L_SCOPE_MARKER("DescriptorSet creation");

      _desc_set_pairings.push();
      working_pairing = &_desc_set_pairings.back();
      working_pairing->camera = &camera;
      working_pairing->pipeline = pipeline;
      working_pairing->last_framebuffer_update = 0;

      if(!Vulkan::find_desc_set(pipeline, working_pairing->desc_set)) {
        VkDescriptorSetLayout layouts[] = {pipeline.desc_set_layout()};
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = Vulkan::descriptor_pool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts;

        L_VK_CHECKED(vkAllocateDescriptorSets(Vulkan::device(), &allocInfo, &working_pairing->desc_set));
      }
    }

    // Bind uniform buffers
    pipeline.set_descriptor("Shared", working_pairing->desc_set, camera.shared_uniform().descriptor_info());
    for(uint32_t i = 0; i < _buffers.size(); i++) {
      if(_buffers[i]) {
        pipeline.set_descriptor(i, working_pairing->desc_set, _buffers[i]->descriptor_info());
      }
    }

    // Set other descriptor set state
    working_pairing->valid = fill_desc_set(pipeline, working_pairing->desc_set);
  } else {
    // Check hash of the final descriptor state
    const uint32_t descriptor_h = _final_state.descriptor_hash();

    if(_last_descriptor_hash != descriptor_h) {
      _last_descriptor_hash = descriptor_h;
      // Update all descriptor sets
      for(const DescSetPairing& pairing : _desc_set_pairings) {
        working_pairing->valid = fill_desc_set(pipeline, pairing.desc_set);
      }
    }
  }

  if(!working_pairing->valid) {
    return nullptr; // Desc set is invalid if some resources are unloaded
  }

  if(&pipeline.render_pass() == &RenderPass::light_pass()
    && working_pairing->last_framebuffer_update < camera.framebuffer_mtime()) {
    pipeline.set_descriptor("color_buffer", working_pairing->desc_set, VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    pipeline.set_descriptor("normal_buffer", working_pairing->desc_set, VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    pipeline.set_descriptor("depth_buffer", working_pairing->desc_set, VkDescriptorImageInfo {Vulkan::sampler(), camera.geometry_buffer().image_view(2), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->last_framebuffer_update = camera.framebuffer_mtime();
  } else if(&pipeline.render_pass() == &RenderPass::present_pass()
    && working_pairing->last_framebuffer_update < camera.framebuffer_mtime()) {
    pipeline.set_descriptor("light_buffer", working_pairing->desc_set, VkDescriptorImageInfo {Vulkan::sampler(), camera.light_buffer().image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    working_pairing->last_framebuffer_update = camera.framebuffer_mtime();
  }
  return working_pairing->desc_set;
}
bool Material::fill_desc_set(const Pipeline& pip, VkDescriptorSet desc_set) const {
  for(const auto& pair : _final_state.scalars) {
    if(const Shader::Binding* binding = pip.find_binding(pair.key())) {
      _buffers[binding->binding]->load_item(pair.value(), binding->offset);
    }
  }
  for(const auto& pair : _final_state.textures) {
    if(pair.value().is_loaded()) {
      pip.set_descriptor(pair.key(), desc_set, VkDescriptorImageInfo {Vulkan::sampler(), *pair.value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    } else {
      return false;
    }
  }
  for(const auto& pair : _final_state.vectors) {
    if(const Shader::Binding* binding = pip.find_binding(pair.key())) {
      _buffers[binding->binding]->load_item(pair.value(), binding->offset);
    }
  }

  if(_final_state.font.is_set()) {
    if(_final_state.font.is_loaded()) {
      pip.set_descriptor("atlas", desc_set, VkDescriptorImageInfo {Vulkan::sampler(), _final_state.font->atlas(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    } else {
      return false;
    }
  }

  return true;
}
void Material::create_uniform_buffers(const Pipeline& pipeline) {
  for(const Shader::Binding& binding : pipeline.bindings()) {
    // Binding 0 is reserved for Shared uniform buffer
    if(binding.binding > 0 && binding.type == Shader::BindingType::Uniform) {
      while(int32_t(_buffers.size()) <= binding.binding) {
        _buffers.push(nullptr);
      }
      if(binding.size > 0) {
        _buffers[binding.binding] = Memory::new_type<GPUBuffer>(binding.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
      }
    }
  }
}
void Material::clear_desc_set_pairings() {
  for(const auto& pairing : _desc_set_pairings) {
    Vulkan::destroy_desc_set(pairing.pipeline, pairing.desc_set);
  }
  for(GPUBuffer* buffer : _buffers) {
    if(buffer) {
      Memory::delete_type(buffer);
    }
  }
  _desc_set_pairings.clear();
  _buffers.clear();
}

Material::~Material() {
  clear_desc_set_pairings();
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
      create_uniform_buffers(*_pipeline);
      return;
    }

    // Clear all existing desc sets as they're no longer valid
    clear_desc_set_pairings();

    // Check shaders
    if(_final_state.pipeline.shaders.empty()) {
      return; // Abort because there aren't any shaders
    }
    for(const auto& shader : _final_state.pipeline.shaders) {
      if(!shader.value().is_loaded()) {
        return; // Abort because some shaders aren't loaded yet
      }
    }

    // Apply vertex attributes to pipeline parameters
    if(_final_state.mesh) {
      _final_state.pipeline.vertex_attributes = _final_state.mesh->attributes();
    }

    // Create pipeline
    pipeline_cache[pipeline_h] = _pipeline = ref<Pipeline>(_final_state.pipeline);

    create_uniform_buffers(*_pipeline);
  }
}
void Material::draw(const Camera& camera, const RenderPass& render_pass, const Matrix44f& model) {
  if(!_pipeline) {
    return;
  }

  L_ASSERT(&_pipeline->render_pass() == &render_pass);
  const VkDescriptorSet desc_set = descriptor_set(camera, *_pipeline);

  if(desc_set == VK_NULL_HANDLE) {
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
  vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline, 0, 1, &desc_set, 0, nullptr);
  if(const Shader::Binding* constants_binding = _pipeline->find_binding("Constants")) {
    vkCmdPushConstants(cmd_buffer, *_pipeline, constants_binding->stage, 0, sizeof(model), &model);
  }

  // Only set scissor if value is different from default
  if(_final_state.scissor != default_scissor) {
    const Vector2f scissor_size = _final_state.scissor.size();
    VkRect2D vk_scissor = {
      VkOffset2D {_final_state.scissor.min().x(), _final_state.scissor.min().y()},
      VkExtent2D {uint32_t(scissor_size.x()), uint32_t(scissor_size.y())},
    };
    vkCmdSetScissor(cmd_buffer, 0, 1, &vk_scissor);
  }

  vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline);
  if(mesh) {
    mesh->draw(cmd_buffer, _final_state.vertex_count, _final_state.index_offset, _final_state.vertex_offset);
  } else if(uint32_t vertex_count = _final_state.vertex_count) {
    vkCmdDraw(cmd_buffer, vertex_count, 1, _final_state.vertex_offset, 0);
  }

  // Reset scissor if we set it earlier
  if(_final_state.scissor != default_scissor) {
    Vulkan::reset_scissor(cmd_buffer);
  }
}
void Material::set_buffer(const Symbol& name, const void* data, size_t size) {
  if(_pipeline) {
    if(const Shader::Binding* binding = _pipeline->find_binding(name)) {
      if(binding->type == Shader::BindingType::Uniform) {
        if(_buffers[binding->binding] && _buffers[binding->binding]->size() != size) {
          Memory::delete_type(_buffers[binding->binding]);
          _buffers[binding->binding] = nullptr;
        }
        if(!_buffers[binding->binding]) {
          _buffers[binding->binding] = Memory::new_type<GPUBuffer>(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        }
        _buffers[binding->binding]->load(data, size);

        // Update live descriptor sets
        for(DescSetPairing& desc_set_pairing : _desc_set_pairings) {
          _pipeline->set_descriptor(binding->binding, desc_set_pairing.desc_set, _buffers[binding->binding]->descriptor_info());
        }
      }
    }
  }
}

bool Material::valid_for_render_pass(const class RenderPass& render_pass) const {
  return _pipeline && &_pipeline->render_pass() == &render_pass;
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

template <class K, class V> static void set_parameter(Array<KeyValue<K, V>>& array, const K& name, const V& value) {
  for(auto& pair : array) {
    if(pair.key() == name) {
      pair.value() = value;
      return;
    }
  }
  array.push(name, value);
}
void Material::shader(VkShaderStageFlags stage, const Resource<Shader>& shader) {
  set_parameter(_partial_state.pipeline.shaders, stage, shader);
  _state_dirty = true;
}
void Material::scalar(const Symbol& name, float scalar) {
  set_parameter(_partial_state.scalars, name, scalar);
  _state_dirty = true;
}
void Material::texture(const Symbol& name, const Resource<Texture>& texture) {
  set_parameter(_partial_state.textures, name, texture);
  _state_dirty = true;
}
void Material::vector(const Symbol& name, const Vector4f& vector) {
  set_parameter(_partial_state.vectors, name, vector);
  _state_dirty = true;
}

static const Symbol frag_symbol("frag"), fragment_symbol("fragment"), vert_symbol("vert"), vertex_symbol("vertex");
static VkShaderStageFlags symbol_to_stage(const Symbol& sym) {
  if(sym == frag_symbol || sym == fragment_symbol) {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  } else if(sym == vert_symbol || sym == vertex_symbol) {
    return VK_SHADER_STAGE_VERTEX_BIT;
  } else {
    return VK_SHADER_STAGE_ALL;
  }
}

static const Symbol fill_symbol("fill"), line_symbol("line");
static VkPolygonMode symbol_to_polygon_mode(const Symbol& sym) {
  if(sym == fill_symbol) {
    return VK_POLYGON_MODE_FILL;
  } else if(sym == line_symbol) {
    return VK_POLYGON_MODE_LINE;
  } else {
    return VK_POLYGON_MODE_MAX_ENUM;
  }
}

static const Symbol back_symbol("back"), front_symbol("front"), none_symbol("none");
static VkCullModeFlags symbol_to_cull_mode(const Symbol& sym) {
  if(sym == back_symbol) {
    return VK_CULL_MODE_BACK_BIT;
  } else if(sym == front_symbol) {
    return VK_CULL_MODE_FRONT_BIT;
  } else if(sym == none_symbol) {
    return VK_CULL_MODE_NONE;
  } else {
    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
  }
}

static const Symbol
point_list_symbol("point_list"),
line_list_symbol("line_list"),
line_strip_symbol("line_strip"),
triangle_list_symbol("triangle_list");
static VkPrimitiveTopology symbol_to_topology(const Symbol& sym) {
  if(sym == point_list_symbol) {
    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  } else if(sym == line_list_symbol) {
    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  } else if(sym == line_strip_symbol) {
    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  } else if(sym == triangle_list_symbol) {
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  } else {
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  }
}

static const Symbol mult_symbol("mult");
static BlendMode symbol_to_blend_mode(const Symbol& sym) {
  if(sym == mult_symbol) {
    return BlendMode::Mult;
  } else {
    return BlendMode::None;
  }
}

void Material::script_registration() {
  L_SCRIPT_METHOD(Material, "parent", 1, parent(c.param(0).get<String>()));
  // Pipeline state
  L_SCRIPT_METHOD(Material, "shader", 2, shader(symbol_to_stage(c.param(0)), c.param(1).get<String>()));
  L_SCRIPT_METHOD(Material, "render_pass", 1, render_pass(c.param(0)));
  L_SCRIPT_METHOD(Material, "polygon_mode", 1, polygon_mode(symbol_to_polygon_mode(c.param(0))));
  L_SCRIPT_METHOD(Material, "cull_mode", 1, cull_mode(symbol_to_cull_mode(c.param(0))));
  L_SCRIPT_METHOD(Material, "topology", 1, topology(symbol_to_topology(c.param(0))));
  L_SCRIPT_METHOD(Material, "blend_mode", 1, blend_mode(symbol_to_blend_mode(c.param(0))));
  // Descriptor state
  L_SCRIPT_METHOD(Material, "texture", 2, texture(c.param(0), c.param(1).get<String>()));
  L_SCRIPT_METHOD(Material, "vector", 2, vector(c.param(0), c.param(1)));
  L_SCRIPT_METHOD(Material, "scalar", 2, scalar(c.param(0), c.param(1)));
  L_SCRIPT_METHOD(Material, "color", 2, color(c.param(0), c.param(1).get<Color>()));
  L_SCRIPT_METHOD(Material, "font", 1, font(c.param(0).get<String>()));
  // Dynamic state
  L_SCRIPT_METHOD(Material, "mesh", 1, mesh(c.param(0).get<String>()));
  L_SCRIPT_METHOD(Material, "text", 1, text(c.param(0).get<String>()));
  L_SCRIPT_METHOD(Material, "vertex_count", 1, vertex_count(uint32_t(c.param(0).get<float>())));
}