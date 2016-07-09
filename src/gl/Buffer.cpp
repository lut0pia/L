#include "Buffer.h"

using namespace L;
using namespace GL;

Buffer::Buffer(GLuint target) : _target(target) {
  glCreateBuffers(1,&_id);
}
Buffer::Buffer(GLuint target,GLsizeiptr size,const void* d,GLuint usage,GLuint index) : Buffer(target) {
  data(size,d,usage);
  if(index>=0) bindBase(index);
}
Buffer::~Buffer() {
  glDeleteBuffers(1,&_id);
}
void Buffer::bind() {
  glBindBuffer(_target,_id);
}
void Buffer::unbind(){
  glBindBuffer(_target,0);
}
void Buffer::data(GLsizeiptr size,const void* data,GLuint usage) {
  glNamedBufferData(_id,size,data,usage);
}
void Buffer::subData(GLintptr offset,GLsizeiptr size,const void* data) {
  glNamedBufferSubData(_id,offset,size,data);
}
void Buffer::bindBase(GLuint index){
  glBindBufferBase(_target,index,_id);
}

