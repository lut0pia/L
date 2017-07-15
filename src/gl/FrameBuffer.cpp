#include "FrameBuffer.h"

#include "Texture.h"
#include "../stream/CFileStream.h"

using namespace L;
using namespace GL;

FrameBuffer::FrameBuffer(GLuint target) : _target(target) {
  glCreateFramebuffers(1,&_id);
}
FrameBuffer::FrameBuffer(GLuint target,const std::initializer_list<Texture*>& colors,Texture* depth,const GLenum* attachments,GLsizei attachmentCount) : FrameBuffer(target) {
  GLuint colorAttachment(GL_COLOR_ATTACHMENT0);
  for(auto&& color : colors)
    attach(colorAttachment++,*color);
  if(depth)
    attach(GL_DEPTH_ATTACHMENT,*depth);
  glNamedFramebufferDrawBuffers(_id,attachmentCount,attachments);
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
void FrameBuffer::attach(GLenum attachment,const Texture& texture) {
  glNamedFramebufferTexture(_id,attachment,texture.id(),0);
}
bool FrameBuffer::check(){
  L_ASSERT(glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  return true;
}