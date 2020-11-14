#pragma once

#include <cstdint>

#include "../math/Interval.h"
#include "../math/Vector.h"
#include "../stream/serial_bin.h"
#include "../text/Symbol.h"

namespace L {
  enum class RenderFormat : uint8_t {
    Undefined,

    R8_UNorm,
    R8G8B8A8_UNorm,
    B8G8R8A8_UNorm,

    D16_UNorm,
    D16_UNorm_S8_UInt,
    R16_SFloat,
    R16_SInt,
    R16_UInt,
    R16G16_SFloat,
    R16G16_SInt,
    R16G16_UInt,
    R16G16B16_SFloat,
    R16G16B16_SInt,
    R16G16B16_UInt,
    R16G16B16A16_SFloat,
    R16G16B16A16_SInt,
    R16G16B16A16_UInt,
    R16G16B16A16_UNorm,

    D24_UNorm_S8_UInt,

    D32_SFloat,
    D32_SFloat_S8_UInt,
    R32_SFloat,
    R32_SInt,
    R32_UInt,
    R32G32_SFloat,
    R32G32_SInt,
    R32G32_UInt,
    R32G32B32_SFloat,
    R32G32B32_SInt,
    R32G32B32_UInt,
    R32G32B32A32_SFloat,
    R32G32B32A32_SInt,
    R32G32B32A32_UInt,

    BC1_RGB_UNorm_Block,
    BC3_UNorm_Block,
  };

  enum class ShaderStage : uint8_t {
    Undefined, Vertex, Fragment, Geometry,
  };

  enum class PolygonMode : uint8_t {
    Undefined, Fill, Line, Point,
  };

  enum class CullMode : uint8_t {
    Undefined, None, Front, Back, FrontAndBack,
  };

  enum class BlendMode : uint8_t {
    Undefined, None, Mult, Add, Alpha,
  };

  enum class PrimitiveTopology : uint8_t {
    Undefined,
    PointList,
    LineList, LineStrip,
    TriangleList, TriangleStrip, TriangleFan,
  };

  enum class VertexAttributeType : uint8_t {
    Undefined, Position, Normal, Tangent, TexCoord, Color, Joints, Weights,
  };

  struct VertexAttribute {
    RenderFormat format = RenderFormat::Undefined;
    VertexAttributeType type = VertexAttributeType::Undefined;
  };

  enum class ShaderBindingType : uint8_t {
    Undefined,
    UniformConstant,
    Uniform,
    Input,
    PushConstant,
  };

  struct ShaderBinding {
    Symbol name;
    int32_t offset = 0, size = 0, index = 0, binding = 0;
    ShaderBindingType type = ShaderBindingType::Undefined;
    RenderFormat format = RenderFormat::Undefined;
    ShaderStage stage = ShaderStage::Undefined;
  };

  struct DescriptorSetImpl {};
  struct FramebufferImpl {};
  struct MeshImpl {};
  struct PipelineImpl {};
  struct RenderCommandBuffer {};
  struct RenderPassImpl {};
  struct ShaderImpl {};
  struct TextureImpl {};
  struct UniformBufferImpl {};

  class Renderer {
  protected:
    static Renderer* _instance;
  public:
    Renderer();
    virtual ~Renderer() {}

    virtual void init(const char* wmid, uintptr_t data1, uintptr_t data2) = 0;
    virtual void recreate_swapchain() = 0;
    virtual void draw(RenderCommandBuffer*, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;

    virtual RenderCommandBuffer* begin_render_command_buffer() = 0;
    virtual void end_render_command_buffer() = 0;
    virtual void begin_present_pass() = 0;
    virtual void end_present_pass() = 0;

    virtual void set_scissor(RenderCommandBuffer*, const Interval2i&) = 0;
    virtual void reset_scissor(RenderCommandBuffer*) = 0;

    virtual void set_viewport(RenderCommandBuffer*, const Interval2i&) = 0;
    virtual void reset_viewport(RenderCommandBuffer*) = 0;

    virtual FramebufferImpl* create_framebuffer(uint32_t width, uint32_t height, const class RenderPass& render_pass) = 0;
    virtual void destroy_framebuffer(FramebufferImpl*) = 0;
    virtual void begin_framebuffer(FramebufferImpl*, RenderCommandBuffer*) = 0;
    virtual void end_framebuffer(FramebufferImpl*, RenderCommandBuffer*) = 0;

    virtual UniformBufferImpl* create_uniform_buffer(size_t size) = 0;
    virtual void destroy_uniform_buffer(UniformBufferImpl*) = 0;
    virtual void load_uniform_buffer(UniformBufferImpl*, const void* src, size_t size, size_t offset) = 0;

    virtual DescriptorSetImpl* create_descriptor_set(PipelineImpl*) = 0;
    virtual void destroy_descriptor_set(DescriptorSetImpl*, PipelineImpl*) = 0;
    virtual void update_descriptor_set(DescriptorSetImpl*, const ShaderBinding&, TextureImpl*) = 0;
    virtual void update_descriptor_set(DescriptorSetImpl*, const ShaderBinding&, UniformBufferImpl*) = 0;
    virtual void update_descriptor_set(DescriptorSetImpl*, const ShaderBinding&, FramebufferImpl*, int32_t texture_index) = 0;

    virtual ShaderImpl* create_shader(ShaderStage, const void* binary, size_t binary_size) = 0;
    virtual void destroy_shader(ShaderImpl*) = 0;

    virtual PipelineImpl* create_pipeline(
      const ShaderImpl* const* shaders,
      size_t shader_count,
      const ShaderBinding* bindings,
      size_t binding_count,
      const VertexAttribute* vertex_attributes,
      size_t vertex_attribute_count,
      const RenderPass& render_pass,
      PolygonMode polygon_mode,
      CullMode cull_mode,
      PrimitiveTopology topology,
      BlendMode blend_mode) = 0;
    virtual void destroy_pipeline(PipelineImpl*) = 0;
    virtual void bind_pipeline(PipelineImpl*, RenderCommandBuffer*, DescriptorSetImpl*, const float* model) = 0;

    virtual TextureImpl* create_texture(uint32_t width, uint32_t height, RenderFormat format, const void* data, size_t size) = 0;
    virtual void load_texture(TextureImpl* texture, const void* data, size_t size, const Vector3i& offset, const Vector3i& extent) = 0;
    virtual void destroy_texture(TextureImpl* texture) = 0;

    virtual MeshImpl* create_mesh(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t acount, const uint16_t* iarray, size_t icount) = 0;
    virtual void destroy_mesh(MeshImpl* mesh) = 0;
    virtual void draw_mesh(RenderCommandBuffer*, MeshImpl* mesh, uint32_t vertex_count = 0, uint32_t index_offset = 0, uint32_t vertex_offset = 0) = 0;

    virtual RenderPassImpl* create_render_pass(const RenderFormat* formats, size_t format_count, bool present) = 0;
    virtual void destroy_render_pass(RenderPassImpl*) = 0;

    static size_t format_size(RenderFormat);
    static bool is_depth_format(RenderFormat);
    static bool is_block_format(RenderFormat);
    inline static Renderer* get() { return _instance; }
  };

  inline Stream& operator<=(Stream& s, const ShaderBinding& v) { return s <= v.name <= v.offset <= v.size <= v.index <= v.binding <= v.type <= v.format <= v.stage; }
  inline Stream& operator>=(Stream& s, ShaderBinding& v) { return s >= v.name >= v.offset >= v.size >= v.index >= v.binding >= v.type >= v.format >= v.stage; }

}
