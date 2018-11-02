#pragma once

#include "../container/Array.h"
#include "../stream/serial_bin.h"

namespace L {
  class MeshBuilder {
  private:
    Array<uint8_t> _vertices;
    Array<uint16_t> _indices;
    size_t _vertex_count;

  public:
    constexpr MeshBuilder() : _vertex_count(0) {}
    void reset();
    void add_vertex(const void* vertex, size_t size);
    void compute_normals(uint32_t vertex_offset, uint32_t normal_offset, size_t vertex_size); // Computes normals from vertices
    inline const void* vertices() const { return &_vertices[0]; }
    inline const uint16_t* indices() const { return &_indices[0]; }
    inline size_t vertices_size() const { return _vertices.size(); }
    inline size_t index_count() const { return _indices.size(); }
    inline size_t vertex_count() const { return _vertex_count; }

    friend inline Stream& operator<=(Stream& s, const MeshBuilder& v) { return s <= v._vertices <= v._indices <= v._vertex_count; }
    friend inline Stream& operator>=(Stream& s, MeshBuilder& v) { return s >= v._vertices >= v._indices >= v._vertex_count; }
  };
}
