#pragma once

#include "RenderPass.h"
#include "Shader.h"

namespace L {
  class Pipeline {
    L_NOCOPY(Pipeline)
  public:
    enum BlendOverride {
      None, Mult,
    };
  protected:
    VkPipeline _pipeline;
    VkPipelineLayout _layout;
    VkDescriptorSetLayout _desc_set_layout;
    Array<Shader::Binding> _bindings;
    const RenderPass& _render_pass;
  public:
    typedef Pipeline* Intermediate;
    Pipeline(const Shader** shaders, size_t count, VkCullModeFlagBits cull_mode, BlendOverride blend_override, const RenderPass& render_pass);
    ~Pipeline();

    const Shader::Binding* find_binding(const Symbol& name) const;

    operator VkPipeline() const { return _pipeline; }
    operator VkPipelineLayout() const { return _layout; }
    inline VkDescriptorSetLayout desc_set_layout() const { return _desc_set_layout; }
    inline const Array<Shader::Binding>& bindings() const { return _bindings; }
    inline const RenderPass& render_pass() const { return _render_pass; }
  };
}
