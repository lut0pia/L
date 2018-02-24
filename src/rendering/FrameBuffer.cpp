#include "FrameBuffer.h"

#include "Texture.h"
#include "../stream/CFileStream.h"

using namespace L;

FrameBuffer::FrameBuffer() {
  glCreateFramebuffers(1, &_id);
}
FrameBuffer::FrameBuffer(const std::initializer_list<Texture*>& colors, Texture* depth) : FrameBuffer() {
  static GLenum attachments[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5};
  GLuint colorAttachment(GL_COLOR_ATTACHMENT0);
  for(Texture* color : colors)
    attach(colorAttachment++, *color);
  if(depth)
    attach(GL_DEPTH_ATTACHMENT, *depth);
  glNamedFramebufferDrawBuffers(_id, GLsizei(colors.size()), attachments);
}
FrameBuffer::~FrameBuffer() {
  glDeleteFramebuffers(1, &_id);
}
void FrameBuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _id);
}
void FrameBuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::attach(GLenum attachment, const Texture& texture) {
  glNamedFramebufferTexture(_id, attachment, texture.id(), 0);
}
bool FrameBuffer::check() {
  L_ASSERT(glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  return true;
}
