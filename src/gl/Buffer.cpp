#include "Buffer.h"

using namespace L;
using namespace GL;

Buffer::Buffer(GLuint target) : _target(target) {
  glGenBuffers(1, &_id);
}
Buffer::~Buffer() {
  glDeleteBuffers(1, &_id);
}
void Buffer::bind() {
  glBindBuffer(_target, _id);
}
void Buffer::data(GLsizeiptr size, const void* data, GLuint usage) {
  bind();
  glBufferData(_target,size,data,usage);
}
void Buffer::subData(GLintptr offset, GLsizeiptr size, const void* data) {
  bind();
  glBufferSubData(_target,offset,size,data);
}

