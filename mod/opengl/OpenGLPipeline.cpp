#include "OpenGLRenderer.h"

#include <L/src/rendering/RenderPass.h>
#include <L/src/rendering/Shader.h>

using namespace L;

PipelineImpl* OpenGLRenderer::create_pipeline(
  const L::ShaderImpl* const* shaders,
  size_t shader_count,
  const ShaderBinding* bindings,
  size_t binding_count,
  const L::VertexAttribute*,
  size_t,
  const L::RenderPass&,
  L::PolygonMode polygon_mode,
  L::CullMode cull_mode,
  L::PrimitiveTopology topology,
  L::BlendMode blend_mode) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLPipeline* pipeline = Memory::new_type<OpenGLPipeline>();

  pipeline->id = glCreateProgram();
  pipeline->polygon_mode = to_gl_polygon_mode(polygon_mode);
  pipeline->cull_mode = to_gl_cull_mode(cull_mode);
  pipeline->topology = to_gl_topology(topology);
  pipeline->blend_src = to_gl_blend_src(blend_mode);
  pipeline->blend_dst = to_gl_blend_dst(blend_mode);

  for(uintptr_t i = 0; i < binding_count; i++) {
    pipeline->bindings.push(bindings[i]);
  }

  for(uintptr_t i = 0; i < shader_count; i++) {
    glAttachShader(pipeline->id, (GLuint)(uintptr_t)shaders[i]);
  }

  glLinkProgram(pipeline->id);

  GLint link_status;
  glGetProgramiv(pipeline->id, GL_LINK_STATUS, &link_status);
  if(link_status != GL_TRUE) {
    GLchar info_log[4096];
    GLsizei info_log_length;
    glGetProgramInfoLog(pipeline->id, sizeof(info_log), &info_log_length, info_log);
    error("opengl: Could not link program: %s", info_log);

    glDeleteProgram(pipeline->id);
  }

  return pipeline;
}
void OpenGLRenderer::destroy_pipeline(PipelineImpl* pipeline) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLPipeline* gl_pipeline = (OpenGLPipeline*)pipeline;
  glDeleteProgram(gl_pipeline->id);
  Memory::delete_type(gl_pipeline);
}
