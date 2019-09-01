#include "MeshBuilder.h"

#include "../math/Vector.h"

using namespace L;

void MeshBuilder::reset() {
  _vertices.clear();
  _indices.clear();
  _vertex_count = 0;
}
void MeshBuilder::add_vertex(const void* vertex) {
  // Search if vertex already exists
  uintptr_t index(0);
  while(index < _vertex_count && memcmp(&_vertices[_vertex_size*index], vertex, _vertex_size)) {
    index++;
  }

  // Vertex does not exist, add it
  if(index == _vertex_count) {
    _vertices.size(_vertices.size() + _vertex_size);
    memcpy(&_vertices[_vertex_count*_vertex_size], vertex, _vertex_size);
    _vertex_count++;
  }

  // Add vertex index to list of indices
  _indices.push(uint16_t(index));
}
void MeshBuilder::compute_normals() {
  L_ASSERT(position_offset != UINTPTR_MAX
    && normal_offset != UINTPTR_MAX);

  // Reset all normals
  for(uint16_t i(0); i < _vertex_count; i++) {
    get_normal(i) = Vector3f(0.f, 0.f, 0.f);
  }

  // Iterate over triangles
  for(uintptr_t i(0); i < _indices.size(); i += 3) {
    // Compute triangle normal
    const Vector3f& a(get_position(_indices[i]));
    Vector3f n((get_position(_indices[i + 1]) - a)
      .cross(get_position(_indices[i + 2]) - a));
    n.normalize();

    // Add to all triangle vertex normals
    get_normal(_indices[i]) += n;
    get_normal(_indices[i + 1]) += n;
    get_normal(_indices[i + 2]) += n;
  }

  // Normalize all normals
  for(uint16_t i(0); i < _vertex_count; i++) {
    get_normal(i).normalize();
  }
}
void MeshBuilder::compute_tangents() {
  L_ASSERT(position_offset != UINTPTR_MAX
    && normal_offset != UINTPTR_MAX
    && tangent_offset != UINTPTR_MAX
    && uv_offset != UINTPTR_MAX);

  // Reset all tangents
  for(uint16_t i(0); i < _vertex_count; i++) {
    get_tangent(i) = Vector3f(0.f, 0.f, 0.f);
  }

  // Iterate over triangles
  for(uintptr_t i(0); i < _indices.size(); i += 3) {
    const Vector3f& p0 = get_position(_indices[i + 0]);
    const Vector3f& p1 = get_position(_indices[i + 1]);
    const Vector3f& p2 = get_position(_indices[i + 2]);

    const Vector2f& uv0 = get_uv(_indices[i + 0]);
    const Vector2f& uv1 = get_uv(_indices[i + 1]);
    const Vector2f& uv2 = get_uv(_indices[i + 2]);

    const Vector3f dp1 = p1 - p0;
    const Vector3f dp2 = p2 - p0;

    const Vector2f duv1 = uv1 - uv0;
    const Vector2f duv2 = uv2 - uv0;

    const float r = 1.f / (duv1.x() * duv2.y() - duv2.x() * duv1.y());
    const Vector3f sdir(
      (duv2.y() * dp1.x() - duv1.y() * dp2.x()) * r,
      (duv2.y() * dp1.y() - duv1.y() * dp2.y()) * r,
      (duv2.y() * dp1.z() - duv1.y() * dp2.z()) * r);
    const Vector3f tdir(
      (duv1.x() * dp2.x() - duv2.x() * dp1.x()) * r,
      (duv1.x() * dp2.y() - duv2.x() * dp1.y()) * r,
      (duv1.x() * dp2.z() - duv2.x() * dp1.z()) * r);

    get_tangent(_indices[i + 0]) += sdir;
    get_tangent(_indices[i + 1]) += sdir;
    get_tangent(_indices[i + 2]) += sdir;
  }

  // Normalize all tangents
  for(uint16_t i(0); i < _vertex_count; i++) {
    get_tangent(i).normalize();
  }
}
