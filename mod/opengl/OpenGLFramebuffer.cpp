#include "OpenGLRenderer.h"

#include <L/src/rendering/RenderPass.h>

using namespace L;

static GLenum attachments[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5};
FramebufferImpl* OpenGLRenderer::create_framebuffer(uint32_t width, uint32_t height, const L::RenderPass& render_pass) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLFramebuffer* framebuffer = Memory::new_type<OpenGLFramebuffer>();
  glCreateFramebuffers(1, &framebuffer->id);

  GLuint color_attachment = GL_COLOR_ATTACHMENT0;
  for(RenderFormat format : render_pass.formats()) {
    OpenGLTexture* texture = (OpenGLTexture*)create_texture(width, height, format, nullptr, 0);
    const bool depth_format = is_depth_format(format);
    GLuint attachement = depth_format ? GL_DEPTH_ATTACHMENT : color_attachment++;
    glNamedFramebufferTexture(framebuffer->id, attachement, texture->id, 0);
    framebuffer->textures.push(texture);

    framebuffer->has_depth |= depth_format;
  }

  glNamedFramebufferDrawBuffers(framebuffer->id, color_attachment - GL_COLOR_ATTACHMENT0, attachments);

  L_ASSERT(glCheckNamedFramebufferStatus(framebuffer->id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
