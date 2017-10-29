#pragma once

#include "../container/Array.h"
#include "GL.h"

namespace L {
  namespace GL {
    class MeshBuilder {
    private:
      Array<byte> _vertices;
      Array<GLushort> _indices;
      size_t _vertexCount;

    public:
      constexpr MeshBuilder() : _vertexCount(0) {}
      void reset();
      void addVertex(const void* vertex, size_t size);
      void computeNormals(uint32_t vertexOffset, uint32_t normalOffset, size_t vertexSize); // Computes normals from vertices
      inline const void* vertices() const { return &_vertices[0]; }
      inline const GLushort* indices() const { return &_indices[0]; }
      inline size_t verticesSize() const { return _vertices.size(); }
      inline size_t indexCount() const { return _indices.size(); }
      inline size_t vertexCount() const { return _vertexCount; }
    };
  }
}
