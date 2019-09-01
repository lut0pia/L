#pragma once

#include "../container/Array.h"
#include "../math/Vector.h"

namespace L {
  class MeshBuilder {
  private:
    Array<uint8_t> _vertices;
    Array<uint16_t> _indices;
    size_t _vertex_count = 0;
    const size_t _vertex_size;

    template <class T> inline T& get_attribute(uint16_t index, uintptr_t offset) {
      return *(T*)(_vertices.begin() + _vertex_size*index + offset);
    }

    inline Vector3f& get_position(uint16_t index) { return get_attribute<Vector3f>(index, position_offset); }
    inline Vector3f& get_normal(uint16_t index) { return get_attribute<Vector3f>(index, normal_offset); }
    inline Vector3f& get_tangent(uint16_t index) { return get_attribute<Vector3f>(index, tangent_offset); }
    inline Vector2f& get_uv(uint16_t index) { return get_attribute<Vector2f>(index, uv_offset); }

  public:
    constexpr MeshBuilder(size_t vertex_size) : _vertex_size(vertex_size) {}
    void reset();
    void add_vertex(const void* vertex);
    void compute_normals();
    void compute_tangents();

    uintptr_t position_offset = UINTPTR_MAX;
    uintptr_t normal_offset = UINTPTR_MAX;
    uintptr_t tangent_offset = UINTPTR_MAX;
    uintptr_t uv_offset = UINTPTR_MAX;

    inline const void* vertices() const { return &_vertices[0]; }
    inline const uint16_t* indices() const { return &_indices[0]; }
    inline size_t vertices_size() const { return _vertices.size(); }
    inline size_t index_count() const { return _indices.size(); }
    inline size_t vertex_count() const { return _vertex_count; }
  };
}
