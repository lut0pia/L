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
  _offsetVertex = Mesh::attributeOffset(_vertexDesc,Mesh::VERTEX);
  _offsetColor = Mesh::attributeOffset(_vertexDesc,Mesh::COLOR);
  _offsetNormal = Mesh::attributeOffset(_vertexDesc,Mesh::NORMAL);
  if(_vertexBufferSize<maxVertices) {
    delete _vertexBuffer;
    _vertexBuffer = new byte[maxVertices*_vertexSize];
    _vertexBufferSize = maxVertices;
  }
  if(_indexBufferSize<maxIndices) {
    delete _indexBuffer;
    _indexBuffer = new uint[maxIndices];
    _indexBufferSize = maxIndices;
  }
  _vertexCount = _indexCount = 0;
  _currentVertex = _vertexBuffer;
}
void MeshBuilder::computeNormals() {
  if(!(_vertexDesc & Mesh::NORMAL))
    throw Exception("There are no normals in this mesh.");
  for(uint i(0); i<_vertexCount; i++)
    normal(i) = Vector3f(0,0,0);
  for(uint i(0); i<_indexCount; i+=3) {
    Vector3f& a(vertex(_indexBuffer[i]));
    Vector3f n((vertex(_indexBuffer[i+1])-a)
               .cross(vertex(_indexBuffer[i+2])-a));
    n.normalize();
    normal(_indexBuffer[i]) += n;
    normal(_indexBuffer[i+1]) += n;
    normal(_indexBuffer[i+2]) += n;
  }
  for(uint i(0); i<_vertexCount; i++)
    normal(i).normalize();
}
void MeshBuilder::setVertex(const Vector3f& vertex) {
  memcpy(_currentVertex+_offsetVertex,vertex.bytes(),sizeof(vertex));
}
void MeshBuilder::setVertexColor(const Color& color) {
  memcpy(_currentVertex+_offsetColor,color.bytes(),sizeof(color));
}
uint MeshBuilder::addVertex() {
  if(_vertexCount>=_vertexBufferSize)
    throw Exception("MeshBuilder vertex buffer too small.");
  _currentVertex += _vertexSize;
  return _vertexCount++;
}
void MeshBuilder::addIndex(uint index) {
  _indexBuffer[_indexCount] = index;
  if(++_indexCount>=_indexBufferSize)
    throw Exception("MeshBuilder index buffer too small.");
}
void MeshBuilder::addTriangle(uint a, uint b, uint c) {
  addIndex(a);
  addIndex(b);
  addIndex(c);
}
