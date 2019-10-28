#pragma once

#include "../container/KeyValue.h"
#include "../engine/Resource.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Mesh.h"

namespace L {
  enum class BlendMode : uint8_t {
    None, Mult,
  };
  class Pipeline {
    L_NOCOPY(Pipeline)
  protected:
    VkPipeline _pipeline;
    VkPipelineLayout _layout;
    VkDescriptorSetLayout _desc_set_layout;
    Array<Shader::Binding> _bindings;
    const RenderPass* _render_pass;
  public:
    struct Parameters {
      Array<KeyValue<VkShaderStageFlags, Resource<Shader>>> shaders;
      Array<VertexAttribute> vertex_attributes;
      Symbol render_pass;
      VkPolygonMode polygon_mode = VK_POLYGON_MODE_MAX_ENUM;
      VkCullModeFlags cull_mode = VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
      VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
      BlendMode blend_mode = BlendMode::None;
    };
    Pipeline(const Parameters& parameters);
    ~Pipeline();

    const Shader::Binding* find_binding(const Symbol& name) const;
    void set_descriptor(uint32_t binding, VkDescriptorSet desc_set, VkDescriptorBufferInfo buffer_info) const;
    void set_descriptor(uint32_t binding, VkDescriptorSet desc_set, VkDescriptorImageInfo image_info) const;
    bool set_descriptor(const Symbol& name, VkDescriptorSet desc_set, VkDescriptorBufferInfo buffer_info) const;
    bool set_descriptor(const Symbol& name, VkDescriptorSet desc_set, VkDescriptorImageInfo image_info) const;

    operator VkPipeline() const { return _pipeline; }
    operator VkPipelineLayout() const { return _layout; }
    inline VkDescriptorSetLayout desc_set_layout() const { return _desc_set_layout; }
    inline const Array<Shader::Binding>& bindings() const { return _bindings; }
    inline const RenderPass& render_pass() const { return *_render_pass; }

    friend inline Stream& operator<=(Stream& s, const Pipeline::Parameters& v) {
      return s <= v.shaders <= v.vertex_attributes <= v.render_pass <= v.polygon_mode <= v.cull_mode <= v.topology <= v.blend_mode;
    }
    friend inline Stream& operator>=(Stream& s, Pipeline::Parameters& v) {
      return s >= v.shaders >= v.vertex_attributes >= v.render_pass >= v.polygon_mode >= v.cull_mode >= v.topology >= v.blend_mode;
    }
  };
}
