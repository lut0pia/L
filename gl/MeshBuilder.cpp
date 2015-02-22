#include "MeshBuilder.h"

#include <cstring>
#include "Mesh.h"

#include "../Exception.h"

using namespace L;
using namespace GL;

MeshBuilder::MeshBuilder()
  : _vertexBuffer(NULL), _vertexBufferSize(0), _vertexCount(0),
    _indexBuffer(NULL), _indexBufferSize(0), _indexCount(0) {
}
MeshBuilder::~MeshBuilder() {
  delete _vertexBuffer;
  delete _indexBuffer;
}
void MeshBuilder::reset(byte vertexDesc, uint maxVertices, uint maxIndices) {
  _vertexDesc = vertexDesc;
  _vertexSize = Mesh::vertexSize(vertexDesc);
  if(_vertexBufferSize<maxVertices) {
    delete _vertexBuffer;
    _vertexBuffer = new float[maxVertices*_vertexSize];
    _vertexBufferSize = maxVertices;
  }
  if(_indexBufferSize<maxIndices) {
    delete _indexBuffer;
    _indexBuffer = new uint[maxIndices];
    _indexBufferSize = maxIndices;
  }
  _vertexCount = _indexCount = 0;
}
void MeshBuilder::setVertex(float x,float y,float z) {
  int offset(Mesh::attributePosition(_vertexDesc,Mesh::VERTEX));
  _currentVertex[offset] = x;
  _currentVertex[offset+1] = y;
  _currentVertex[offset+2] = z;
}
void MeshBuilder::setVertexColor(float r,float g,float b, float a) {
  int offset(Mesh::attributePosition(_vertexDesc,Mesh::COLOR));
  _currentVertex[offset] = r;
  _currentVertex[offset+1] = g;
  _currentVertex[offset+2] = b;
  _currentVertex[offset+3] = a;
}
uint MeshBuilder::addVertex() {
  if(_vertexCount==_vertexBufferSize)
    throw Exception("MeshBuilder vertex buffer too small.");
  int offset(_vertexCount*_vertexSize);
  memcpy(_vertexBuffer+offset,_currentVertex,_vertexSize*sizeof(float));
  return _vertexCount++;
}
void MeshBuilder::addIndex(uint index) {
  _indexBuffer[_indexCount] = index;
  if(++_indexCount==_indexBufferSize)
    throw Exception("MeshBuilder index buffer too small.");
}
void MeshBuilder::addTriangle(uint a, uint b, uint c) {
  addIndex(a);
  addIndex(b);
  addIndex(c);
}
