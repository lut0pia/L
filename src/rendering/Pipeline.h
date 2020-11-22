#pragma once

#include "../container/KeyValue.h"
#include "../engine/Resource.h"
#include "../math/Matrix.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace L {
  class Pipeline {
    L_NOCOPY(Pipeline)
  protected:
    PipelineImpl* _impl;
    Array<ShaderBinding> _bindings;
    const RenderPass* _render_pass;
  public:
    struct Parameters {
      Array<KeyValue<ShaderStage, Resource<Shader>>> shaders;
      Array<VertexAttribute> vertex_attributes;
      Symbol render_pass;
      PolygonMode polygon_mode = PolygonMode::Undefined;
      CullMode cull_mode = CullMode::Undefined;
      PrimitiveTopology topology = PrimitiveTopology::Undefined;
      BlendMode blend_mode = BlendMode::Undefined;
    };
    Pipeline(const Parameters& parameters);
    ~Pipeline();

    const ShaderBinding* find_binding(const Symbol& name) const;
    const ShaderBinding* find_binding(int32_t binding) const;
    bool set_descriptor(int32_t binding, DescriptorSetImpl*, const UniformBuffer&) const;
    bool set_descriptor(int32_t binding, DescriptorSetImpl*, const Texture&) const;
    bool set_descriptor(const Symbol& name, DescriptorSetImpl*, const UniformBuffer&) const;
    bool set_descriptor(const Symbol& name, DescriptorSetImpl*, const Texture&) const;
    bool set_descriptor(const Symbol& name, DescriptorSetImpl*, const Framebuffer&, int32_t texture_index) const;

    inline PipelineImpl* get_impl() const { return _impl; }
    inline const Array<ShaderBinding>& bindings() const { return _bindings; }
    inline const RenderPass& render_pass() const { return *_render_pass; }

    friend inline Stream& operator<=(Stream& s, const Pipeline::Parameters& v) {
      return s <= v.shaders <= v.vertex_attributes <= v.render_pass <= v.polygon_mode <= v.cull_mode <= v.topology <= v.blend_mode;
    }
    friend inline Stream& operator>=(Stream& s, Pipeline::Parameters& v) {
      return s >= v.shaders >= v.vertex_attributes >= v.render_pass >= v.polygon_mode >= v.cull_mode >= v.topology >= v.blend_mode;
    }
  };
}
