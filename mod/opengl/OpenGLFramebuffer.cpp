#include "OpenGLRenderer.h"

using namespace L;

static GLenum attachments[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5};
FramebufferImpl* OpenGLRenderer::create_framebuffer(const L::RenderPassImpl* render_pass, const L::TextureImpl** textures, size_t texture_count) {
  L_SCOPE_THREAD_MASK(1);
  OpenGLRenderPass* gl_render_pass = (OpenGLRenderPass*)render_pass;

  OpenGLFramebuffer* framebuffer = Memory::new_type<OpenGLFramebuffer>();
  glCreateFramebuffers(1, &framebuffer->id);

  if(gl_render_pass->depth_write) {
    framebuffer->clear_bitfield |= GL_DEPTH_BUFFER_BIT;
  }
  framebuffer->depth_mask = gl_render_pass->depth_write;

  GLuint color_attachment = GL_COLOR_ATTACHMENT0;
  for(uintptr_t i = 0; i < texture_count; i++) {
    OpenGLTexture* texture = (OpenGLTexture*)textures[i];
    const bool depth_format = is_depth_format(texture->format);
    GLuint attachement = depth_format ? GL_DEPTH_STENCIL_ATTACHMENT : color_attachment++;
    glNamedFramebufferTexture(framebuffer->id, attachement, texture->id, 0);
    framebuffer->textures.push(texture);

    framebuffer->has_depth |= depth_format;
  }

  glNamedFramebufferDrawBuffers(framebuffer->id, color_attachment - GL_COLOR_ATTACHMENT0, attachments);

  GLenum framebuffer_status = glCheckNamedFramebufferStatus(framebuffer->id, GL_FRAMEBUFFER);
  if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
    error("opengl: Incomplete framebuffer: %s", error_str(framebuffer_status));
  }

  return framebuffer;
}
void OpenGLRenderer::destroy_framebuffer(L::FramebufferImpl* framebuffer) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLFramebuffer* gl_framebuffer = (OpenGLFramebuffer*)framebuffer;

  for(OpenGLTexture* texture : gl_framebuffer->textures) {
    destroy_texture(texture);
  }

  glDeleteFramebuffers(1, &gl_framebuffer->id);

  Memory::delete_type(gl_framebuffer);
}
void OpenGLRenderer::begin_framebuffer(L::FramebufferImpl* framebuffer, L::RenderCommandBuffer*) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLFramebuffer* gl_framebuffer = (OpenGLFramebuffer*)framebuffer;
  glBindFramebuffer(GL_FRAMEBUFFER, gl_framebuffer->id);

  glDepthMask(gl_framebuffer->depth_mask);
  glClear(gl_framebuffer->clear_bitfield);

  if(gl_framebuffer->has_depth) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}
void OpenGLRenderer::end_framebuffer(L::FramebufferImpl*, L::RenderCommandBuffer*) {
  L_SCOPE_THREAD_MASK(1);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
