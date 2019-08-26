#include "MeshBuilder.h"

#include "../math/Vector.h"

using namespace L;

void MeshBuilder::reset() {
  _vertices.clear();
  _indices.clear();
  _vertex_count = 0;
}
void MeshBuilder::add_vertex(const void* vertex, size_t size) {
  uintptr_t index(0);
  while(index<_vertex_count && memcmp(&_vertices[size*index], vertex, size))
    index++;
  if(index==_vertex_count) {
    _vertices.size(_vertices.size()+size);
    memcpy(&_vertices[_vertex_count*size], vertex, size);
    _vertex_count++;
  }
  _indices.push(uint16_t(index));
}
void MeshBuilder::compute_normals(uint32_t vertexOffset, uint32_t normalOffset, size_t vertexSize) {
#define ATTRIBUTE_OF(i,offset) (*(Vector3f*)(_vertices.begin()+vertexSize*i+offset))
#define VERTEX_OF(i) ATTRIBUTE_OF(i,vertexOffset)
#define NORMAL_OF(i) ATTRIBUTE_OF(i,normalOffset)
  for(uint32_t i(0); i<_vertex_count; i++)
    NORMAL_OF(i) = Vector3f(0.f, 0.f, 0.f);
  for(uint32_t i(0); i<_indices.size(); i += 3) {
    const Vector3f& a(VERTEX_OF(_indices[i]));
    Vector3f n((VERTEX_OF(_indices[i+1])-a)
               .cross(VERTEX_OF(_indices[i+2])-a));
    n.normalize();
    NORMAL_OF(_indices[i]) += n;
    NORMAL_OF(_indices[i+1]) += n;
    NORMAL_OF(_indices[i+2]) += n;
  }
  for(uint32_t i(0); i<_vertex_count; i++)
    NORMAL_OF(i).normalize();
}
