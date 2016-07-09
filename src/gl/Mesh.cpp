#include "Mesh.h"

#include "Buffer.h"
#include "MeshBuilder.h"
#include "../Interface.h"
#include "GL.h"

using namespace L;
using namespace GL;


Mesh::Mesh(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>& attributes,const GLushort* iarray,GLsizei icount)
  : Mesh(){
  load(mode,count,data,size,attributes,iarray,icount);
}
Mesh::Mesh(const char* filename) : Mesh(){
  Interface<Mesh>::fromFile(*this,filename);
}
Mesh::~Mesh(){
  if(_vao) glDeleteVertexArrays(1,&_vao);
  if(_vbo) glDeleteBuffers(1,&_vbo);
  if(_eab) glDeleteBuffers(1,&_eab);
  _vao = _vbo = _eab = 0;
}
void Mesh::indices(const GLushort* data,GLsizei count){
  _count = count;
  glCreateBuffers(1,&_eab);
  glNamedBufferData(_eab,count*sizeof(GLushort),data,GL_STATIC_DRAW);
  glVertexArrayElementBuffer(_vao,_eab);
}
void Mesh::load(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>& attributes,const GLushort* iarray,GLsizei icount){
  this->~Mesh();
  _mode = mode;
  _count = count;
  glCreateVertexArrays(1,&_vao);
  glCreateBuffers(1,&_vbo);
  glNamedBufferData(_vbo,size,data,GL_STATIC_DRAW);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER,_vbo);
  for(auto&& a : attributes){
    glEnableVertexArrayAttrib(_vao,a.index);
    glVertexAttribPointer(a.index,a.size,a.type,a.normalized,a.stride,(const void*)a.offset);
    //glVertexArrayVertexBuffer(_vao,a.index,_vbo,a.offset,a.stride+a.size);
    //glVertexArrayAttribFormat(_vao,a.index,a.size,a.type,a.normalized,a.stride+a.size);
    //glVertexArrayAttribBinding(_vao,a.index,a.index);
  }
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
  if(iarray) indices(iarray,icount);
}
void Mesh::load(const MeshBuilder& mb,GLenum mode,const std::initializer_list<Attribute>& attributes){
  load(mode,mb.vertexCount(),mb.vertices(),mb.verticesSize(),attributes,mb.indices(),mb.indexCount());
}
void Mesh::draw() const{
  if(_vao){
    glBindVertexArray(_vao);
    if(_eab) glDrawElements(_mode,_count,GL_UNSIGNED_SHORT,0);
    else glDrawArrays(_mode,0,_count);
  }
}
