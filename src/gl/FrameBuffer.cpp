#include "FrameBuffer.h"

#include "RenderBuffer.h"
#include "Texture.h"

using namespace L;
using namespace GL;

FrameBuffer::FrameBuffer(GLuint target) : _target(target) {
  glGenFramebuffers(1,&_id);
}
FrameBuffer::FrameBuffer(GLuint target,const std::initializer_list<Texture*>& colors,Texture* depth,const GLenum* attachments,GLsizei attachmentCount) : _target(target) {
  glGenFramebuffers(1,&_id);
  GLuint colorAttachment(GL_COLOR_ATTACHMENT0);
  for(auto&& color : colors)
    attach(colorAttachment++,*color);
  attach(GL_DEPTH_ATTACHMENT,*depth);
  glDrawBuffers(attachmentCount,attachments);
  L_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
FrameBuffer::~FrameBuffer() {
  glDeleteFramebuffers(1,&_id);
}
void FrameBuffer::bind() {
  glBindFramebuffer(_target,_id);
}
void FrameBuffer::unbind(){
  glBindFramebuffer(_target,0);
}
void FrameBuffer::attach(GLenum attachment,const RenderBuffer& renderBuffer) {
  bind();
  glFramebufferRenderbuffer(_target,attachment,GL_RENDERBUFFER,renderBuffer.id());
}
void FrameBuffer::attach(GLenum attachment,const Texture& texture) {
  bind();
  glFramebufferTexture2D(_target,attachment,GL_TEXTURE_2D,texture.id(),0);
}
