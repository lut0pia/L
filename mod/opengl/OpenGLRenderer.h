#pragma once

#if L_WINDOWS
#include <Windows.h>
#endif

#include <GL/gl.h>
#ifndef __glext_h_ // Only include bundled glext.h if necessary
#include "glext.h"
#endif

#include <L/src/container/Array.h>
#include <L/src/container/Table.h>
#include <L/src/parallelism/Lock.h>
#include <L/src/rendering/Renderer.h>
#include <L/src/rendering/Shader.h>

struct OpenGLDescriptorSet : public L::DescriptorSetImpl {
  struct OpenGLUniformBuffer* constants = nullptr;
  L::Table<L::ShaderBinding, struct OpenGLTexture*> textures;
  L::Table<L::ShaderBinding, struct OpenGLUniformBuffer*> uniform_buffers;
};

struct OpenGLFramebuffer : public L::FramebufferImpl {
  L::Array<struct OpenGLTexture*> textures;
  GLuint id;
  GLbitfield clear_bitfield = GL_COLOR_BUFFER_BIT;
  bool depth_mask = true;
  bool has_depth = false;
};

struct OpenGLMesh : public L::MeshImpl {
  GLuint vao = 0, vbo = 0, eab = 0;
  GLsizei stride = 0;
};

struct OpenGLPipeline : public L::PipelineImpl {
  GLuint id;
  GLenum polygon_mode;
  GLuint cull_mode;
  GLuint topology;
  GLenum blend_src;
  GLenum blend_dst;
  GLenum depth_func;
  L::Array<L::ShaderBinding> bindings;
};

struct OpenGLRenderPass : public L::RenderPassImpl {
  bool depth_write = false;
};

struct OpenGLTexture : public L::TextureImpl {
  GLuint id;
  GLenum target;
  L::RenderFormat format;
  uint32_t width, height;
};

struct OpenGLUniformBuffer : public L::UniformBufferImpl {
  GLuint id;
};

class OpenGLRenderer : public L::Renderer {
protected:
  // Declare GL functions
#define L_GL_FUNC(type,name) static type name;
#include "gl_functions.def"
#undef L_GL_FUNC
  GLuint _dummy_vao;
  void* _hdc;
  uint32_t _glsl_version;
public:
  virtual bool init(L::GenericWindowData* window_data) override;
  virtual void recreate_swapchain() override;
  virtual void draw(
    L::RenderCommandBuffer*,
    L::PipelineImpl*,
    L::DescriptorSetImpl*,
    const float* model,
    L::MeshImpl*,
    uint32_t vertex_count,
    uint32_t index_offset) override;


  virtual L::RenderCommandBuffer* begin_render_command_buffer() override;
  virtual void end_render_command_buffer() override;
  virtual void begin_present_pass() override;
  virtual void end_present_pass() override;

  virtual void set_scissor(L::RenderCommandBuffer*, const L::Interval2i&) override;
  virtual void reset_scissor(L::RenderCommandBuffer*) override;

  virtual void set_viewport(L::RenderCommandBuffer*, const L::Interval2i&) override;
  virtual void reset_viewport(L::RenderCommandBuffer*) override;

  virtual void begin_event(L::RenderCommandBuffer*, const char*) override;
  virtual void end_event(L::RenderCommandBuffer*) override;

  virtual L::FramebufferImpl* create_framebuffer(const L::RenderPassImpl*, const L::TextureImpl** textures, size_t texture_count) override;
  virtual void destroy_framebuffer(L::FramebufferImpl*) override;
  virtual void begin_framebuffer(L::FramebufferImpl*, L::RenderCommandBuffer*) override;
  virtual void end_framebuffer(L::FramebufferImpl*, L::RenderCommandBuffer*) override;

  virtual L::UniformBufferImpl* create_uniform_buffer(size_t size) override;
  virtual void destroy_uniform_buffer(L::UniformBufferImpl*) override;
  virtual void load_uniform_buffer(L::UniformBufferImpl*, const void* data, size_t size, size_t offset) override;

  virtual L::DescriptorSetImpl* create_descriptor_set(L::PipelineImpl*) override;
  virtual void destroy_descriptor_set(L::DescriptorSetImpl*, L::PipelineImpl*) override;
  virtual void update_descriptor_set(L::DescriptorSetImpl*, const L::ShaderBinding&, L::TextureImpl*) override;
  virtual void update_descriptor_set(L::DescriptorSetImpl*, const L::ShaderBinding&, L::UniformBufferImpl*) override;
  virtual void update_descriptor_set(L::DescriptorSetImpl*, const L::ShaderBinding&, L::FramebufferImpl*, int32_t texture_index) override;

  virtual L::ShaderImpl* create_shader(L::ShaderStage, const void* binary, size_t binary_size) override;
  virtual void destroy_shader(L::ShaderImpl*) override;

  virtual L::PipelineImpl* create_pipeline(
    const L::ShaderImpl* const* shaders,
    size_t shader_count,
    const L::ShaderBinding* bindings,
    size_t binding_count,
    const L::VertexAttribute* vertex_attributes,
    size_t vertex_attribute_count,
    const L::RenderPassImpl* render_pass,
    L::PolygonMode polygon_mode,
    L::CullMode cull_mode,
    L::PrimitiveTopology topology,
    L::BlendMode blend_mode,
    L::DepthFunc depth_func) override;
  virtual void destroy_pipeline(L::PipelineImpl*) override;

  virtual L::TextureImpl* create_texture(uint32_t width, uint32_t height, L::RenderFormat format, const void* data, size_t size) override;
  virtual void load_texture(L::TextureImpl* texture, const void* data, size_t size, const L::Vector3i& offset, const L::Vector3i& extent) override;
  virtual void destroy_texture(L::TextureImpl* texture) override;

  virtual L::MeshImpl* create_mesh(size_t count, const void* data, size_t size, const L::VertexAttribute* attributes, size_t attribute_count, const uint16_t* iarray, size_t icount) override;
  virtual void destroy_mesh(L::MeshImpl* mesh) override;

  virtual L::RenderPassImpl* create_render_pass(const L::RenderFormat* formats, size_t format_count, bool present, bool depth_write) override;
  virtual void destroy_render_pass(L::RenderPassImpl*) override;

  static GLenum to_gl_format(L::RenderFormat);
  static GLint to_gl_size(L::RenderFormat);
  static GLenum to_gl_type(L::RenderFormat);
  static GLint to_gl_internal_format(L::RenderFormat);
  static bool to_gl_normalized(L::RenderFormat);
  static bool to_gl_integer(L::RenderFormat);
  static GLuint to_gl_shader_stage(L::ShaderStage);
  static GLuint to_gl_topology(L::PrimitiveTopology);
  static GLuint to_gl_polygon_mode(L::PolygonMode);
  static GLuint to_gl_cull_mode(L::CullMode);
  static GLuint to_gl_blend_src(L::BlendMode);
  static GLuint to_gl_blend_dst(L::BlendMode);
  static GLuint to_gl_depth_func(L::DepthFunc);

  static void* load_function(const char*);
  static const char* error_str(GLenum result);
  static OpenGLRenderer* get() { return (OpenGLRenderer*)_instance; }
};
