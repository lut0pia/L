#include "GPUBuffer.h"

using namespace L;

GPUBuffer::GPUBuffer(GLuint target) : _target(target) {
  glCreateBuffers(1,&_id);
}
GPUBuffer::GPUBuffer(GLuint target,GLsizeiptr size,const void* d,GLuint usage,GLuint index) : GPUBuffer(target) {
  data(size,d,usage);
  if(index>=0) bindBase(index);
}
GPUBuffer::~GPUBuffer() {
  glDeleteBuffers(1,&_id);
}
void GPUBuffer::bind() {
  glBindBuffer(_target,_id);
}
void GPUBuffer::unbind(){
  glBindBuffer(_target,0);
}
void GPUBuffer::data(GLsizeiptr size,const void* data,GLuint usage) {
  glNamedBufferData(_id,size,data,usage);
}
void GPUBuffer::subData(GLintptr offset,GLsizeiptr size,const void* data) {
  glNamedBufferSubData(_id,offset,size,data);
}
void GPUBuffer::bindBase(GLuint index){
  glBindBufferBase(_target,index,_id);
}

