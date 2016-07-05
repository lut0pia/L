#include "MeshBuilder.h"

using namespace L;
using namespace GL;

void MeshBuilder::reset(){
  _vertices.clear();
  _indices.clear();
  _vertexCount = 0;
}
void MeshBuilder::addVertex(const void* vertex,size_t size){
  uintptr_t index(0);
  while(index<_vertexCount && memcmp(&_vertices[size*index],vertex,size))
    index++;
  if(index==_vertexCount){
    _vertices.size(_vertices.size()+size);
    memcpy(&_vertices[_vertexCount*size],vertex,size);
    _vertexCount++;
  }
  _indices.push(index);
}