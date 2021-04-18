#include "OpenGLRenderer.h"

using namespace L;

DescriptorSetImpl* OpenGLRenderer::create_descriptor_set(PipelineImpl*) {
  return Memory::new_type<OpenGLDescriptorSet>();
}
void OpenGLRenderer::destroy_descriptor_set(DescriptorSetImpl* desc_set, PipelineImpl*) {
  OpenGLDescriptorSet* gl_desc_set = (OpenGLDescriptorSet*)desc_set;
  if(gl_desc_set->constants) {
    destroy_uniform_buffer(gl_desc_set->constants);
  }
  Memory::delete_type(gl_desc_set);
}
void OpenGLRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, TextureImpl* texture) {
  OpenGLDescriptorSet* gl_desc_set = (OpenGLDescriptorSet*)desc_set;
  gl_desc_set->textures[binding] = (OpenGLTexture*)texture;
}
void OpenGLRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, UniformBufferImpl* uniform_buffer) {
  OpenGLDescriptorSet* gl_desc_set = (OpenGLDescriptorSet*)desc_set;
  gl_desc_set->uniform_buffers[binding] = (OpenGLUniformBuffer*)uniform_buffer;
}
void OpenGLRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, FramebufferImpl* framebuffer, int32_t texture_index) {
  OpenGLFramebuffer* gl_framebuffer = (OpenGLFramebuffer*)framebuffer;
  update_descriptor_set(desc_set, binding, gl_framebuffer->textures[texture_index]);
}
