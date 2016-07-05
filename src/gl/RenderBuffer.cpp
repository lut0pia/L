#include "RenderBuffer.h"

using namespace L;
using namespace GL;

RenderBuffer::RenderBuffer() {
  glGenRenderbuffers(1,&_id);
}
RenderBuffer::~RenderBuffer() {
  glDeleteRenderbuffers(1,&_id);
}
void RenderBuffer::bind() {
  glBindRenderbuffer(GL_RENDERBUFFER,_id);
}
void RenderBuffer::storage(GLenum internalformat,GLsizei width,GLsizei height) {
  bind();
  glRenderbufferStorage(GL_RENDERBUFFER,internalformat,width,height);
}
