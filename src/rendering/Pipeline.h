#pragma once

#include "../engine/Resource.h"
#include "RenderPass.h"
#include "Shader.h"

namespace L {
  enum class BlendMode {
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
    struct Intermediate {
      Array<Resource<Shader>> shaders;
      Symbol render_pass;
      VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;
      BlendMode blend_override = BlendMode::None;
    };
    Pipeline(const Intermediate& intermediate);
    ~Pipeline();

    const Shader::Binding* find_binding(const Symbol& name) const;

    operator VkPipeline() const { return _pipeline; }
    operator VkPipelineLayout() const { return _layout; }
    inline VkDescriptorSetLayout desc_set_layout() const { return _desc_set_layout; }
    inline const Array<Shader::Binding>& bindings() const { return _bindings; }
    inline const RenderPass& render_pass() const { return *_render_pass; }
  };
}
