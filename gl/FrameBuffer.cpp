#include "FrameBuffer.h"

using namespace L;
using namespace GL;

FrameBuffer::FrameBuffer(GLuint target) : _target(target) {
  glGenFramebuffers(1, &_id);
}
FrameBuffer::~FrameBuffer() {
  glDeleteFramebuffers(1, &_id);
}
void FrameBuffer::bind() {
  glBindFramebuffer(_target, _id);
}
