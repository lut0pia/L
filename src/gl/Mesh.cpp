#include "Mesh.h"

#include "../Interface.h"

using namespace L;
using namespace GL;

Mesh::Mesh()
  : _vertexDesc(0),
    _vertexBuffer(GL_ARRAY_BUFFER),
    _indexBuffer(GL_ELEMENT_ARRAY_BUFFER),
    _vertexCount(0),
    _vertexSize(0),
    _indexCount(0),
    _primitive(0) {
}
Mesh::Mesh(const String& path)
  : _vertexDesc(0),
    _vertexBuffer(GL_ARRAY_BUFFER),
    _indexBuffer(GL_ELEMENT_ARRAY_BUFFER),
    _vertexCount(0),
    _vertexSize(0),
    _indexCount(0),
    _primitive(0) {
  Interface<Mesh>::fromFile(*this,path);
}
Mesh::Mesh(const MeshBuilder& builder, GLenum primitive)
  : _vertexDesc(builder.vertexDesc()),
    _vertexBuffer(GL_ARRAY_BUFFER),
    _indexBuffer(GL_ELEMENT_ARRAY_BUFFER),
    _vertexCount(builder.vertexCount()),
    _vertexSize(vertexSize(_vertexDesc)),
    _indexCount(builder.indexCount()),
    _primitive(primitive) {
  _vertexBuffer.data(_vertexCount*_vertexSize,builder.vertices(),GL_STATIC_DRAW);
  if(_indexCount)
    _indexBuffer.data(_indexCount*sizeof(uint),builder.indices(),GL_STATIC_DRAW);
}
/*
Mesh::Mesh(byte vertexDesc, float* vertices, GLsizei count, GLenum primitive)
  : _vertexDesc(vertexDesc), _vertexBuffer(GL_ARRAY_BUFFER), _vertexCount(count),
    _vertexSize(vertexSize(_vertexDesc)), _primitive(primitive) {
  _vertexBuffer.data(_vertexCount*_vertexSize*sizeof(float),vertices,GL_STATIC_DRAW);
}
*/

void Mesh::draw() {
  if(!_vertexCount) return; // It's an empty mesh
  _vertexBuffer.bind();
  if(_indexCount)
    _indexBuffer.bind();
  if(_vertexDesc&VERTEX) glEnableClientState(GL_VERTEX_ARRAY);
  if(_vertexDesc&COLOR) glEnableClientState(GL_COLOR_ARRAY);
  if(_vertexDesc&NORMAL) glEnableClientState(GL_NORMAL_ARRAY);
  if(_vertexDesc&VERTEX) glVertexPointer(3, GL_FLOAT, _vertexSize, (void*)(attributeOffset(_vertexDesc,VERTEX)));
  if(_vertexDesc&COLOR) glColorPointer(GL_BGRA, GL_UNSIGNED_BYTE, _vertexSize, (void*)(attributeOffset(_vertexDesc,COLOR)));
  if(_vertexDesc&NORMAL) glNormalPointer(GL_FLOAT, _vertexSize, (void*)(attributeOffset(_vertexDesc,NORMAL)));
  if(_indexCount) glDrawElements(_primitive, _indexCount, GL_UNSIGNED_INT, (void*)0);
  else glDrawArrays(_primitive, 0, _vertexCount);
  if(_vertexDesc&VERTEX) glDisableClientState(GL_VERTEX_ARRAY);
  if(_vertexDesc&COLOR) glDisableClientState(GL_COLOR_ARRAY);
  if(_vertexDesc&NORMAL) glDisableClientState(GL_NORMAL_ARRAY);
}

GLsizei Mesh::vertexSize(byte desc) {
  GLsizei wtr(0);
  if(desc&VERTEX) wtr += 3*sizeof(float);
  if(desc&COLOR) wtr += 4;
  if(desc&NORMAL) wtr += 3*sizeof(float);
  if(desc&TEXCOORD) wtr += 2*sizeof(float);
  return wtr;
}
GLsizei Mesh::attributeOffset(byte desc, byte type) {
  GLsizei wtr(0);
  if(type==VERTEX) return wtr;
  if(desc&VERTEX) wtr += 3*sizeof(float);
  if(type==COLOR) return wtr;
  if(desc&COLOR) wtr += 4;
  if(type==NORMAL) return wtr;
  if(desc&NORMAL) wtr += 3*sizeof(float);
  if(type==TEXCOORD) return wtr;
  if(desc&TEXCOORD) wtr += 2*sizeof(float);
  L_ERROR("Attribute could not be found in vertex description.");
}
