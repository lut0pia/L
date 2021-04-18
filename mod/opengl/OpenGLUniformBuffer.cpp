#include "OpenGLRenderer.h"

using namespace L;

L::UniformBufferImpl* OpenGLRenderer::create_uniform_buffer(size_t size) {
  L_SCOPE_THREAD_MASK(1);
  
  OpenGLUniformBuffer* uniform_buffer = Memory::new_type<OpenGLUniformBuffer>();
  glCreateBuffers(1, &uniform_buffer->id);
  glNamedBufferData(uniform_buffer->id, size, nullptr, GL_DYNAMIC_DRAW);
  return uniform_buffer;
}
void OpenGLRenderer::destroy_uniform_buffer(UniformBufferImpl* uniform_buffer) {
  OpenGLTexture* gl_uniform_buffer = (OpenGLTexture*)uniform_buffer;
  glDeleteBuffers(1, &gl_uniform_buffer->id);
  Memory::delete_type(gl_uniform_buffer);
}
void OpenGLRenderer::load_uniform_buffer(UniformBufferImpl* uniform_buffer, const void* data, size_t size, size_t offset) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLTexture* gl_uniform_buffer = (OpenGLTexture*)uniform_buffer;
  glNamedBufferSubData(gl_uniform_buffer->id, offset, size, data);
}
