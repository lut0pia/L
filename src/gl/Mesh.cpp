#include "Mesh.h"

#include "Buffer.h"
#include "MeshBuilder.h"
#include "../Interface.h"

using namespace L;
using namespace GL;


Mesh::Mesh(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>& attributes,const GLushort* iarray,GLsizei icount)
  : _vao(0),_eab(0){
  load(mode,count,data,size,attributes,iarray,icount);
}
Mesh::Mesh(const char* filename) : _vao(0),_eab(0){
  Interface<Mesh>::fromFile(*this,filename);
}
Mesh::~Mesh(){
  if(_vao){
    if(_eab)
    {
    }
  }
}
void Mesh::indices(const GLushort* data,GLsizei count){
  _count = count;
  glGenBuffers(1,&_eab);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_eab);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,count*sizeof(GLushort),data,GL_STATIC_DRAW);
}
void Mesh::load(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>& attributes,const GLushort* iarray,GLsizei icount){
  if(_vao) this->~Mesh();
  _mode = mode;
  _count = count;
  glGenVertexArrays(1,&_vao);
  glBindVertexArray(_vao);
  glGenBuffers(1,&_vbo);
  glBindBuffer(GL_ARRAY_BUFFER,_vbo);
  glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
  for(auto&& a : attributes){
    glEnableVertexAttribArray(a.index);
    glVertexAttribPointer(a.index,a.size,a.type,a.normalized,a.stride,a.pointer);
  }
  if(iarray) indices(iarray,icount);
}
void Mesh::load(const MeshBuilder& mb,GLenum mode,const std::initializer_list<Attribute>& attributes){
  load(mode,mb.vertexCount(),mb.vertices(),mb.verticesSize(),attributes,mb.indices(),mb.indexCount());
}
void Mesh::draw() const{
  if(_vao){
    glBindVertexArray(_vao);
    if(_eab){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_eab);
      glDrawElements(_mode,_count,GL_UNSIGNED_SHORT,0);
    } else glDrawArrays(_mode,0,_count);
  }
}
