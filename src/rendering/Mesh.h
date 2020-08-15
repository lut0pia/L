#pragma once

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "GPUBuffer.h"
#include "../macros.h"
#include "../math/Interval.h"

namespace L {
  enum class VertexAttributeType : uint8_t {
    Undefined, Position, Normal, Tangent, TexCoord, Color, Joints, Weights,
  };
  struct VertexAttribute {
    VkFormat format;
    VertexAttributeType type;
  };
  class Mesh {
    L_NOCOPY(Mesh)
  private:
    GPUBuffer *_vertex_buffer, *_index_buffer;
    Array<VertexAttribute> _attributes;
    Interval3f _bounds;
    uint32_t _count;

  public:
    struct Intermediate {
      Buffer vertices, indices;
      Array<VertexAttribute> attributes;
    };
    inline Mesh() : _vertex_buffer(nullptr), _index_buffer(nullptr) {}
    Mesh(const Intermediate& intermediate);
    Mesh(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t acount, const uint16_t* indices = nullptr, size_t icount = 0);
    ~Mesh();

    void load(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t acount, const uint16_t* indices = nullptr, size_t icount = 0);
    void draw(VkCommandBuffer, uint32_t vertex_count = 0, uint32_t index_offset = 0, uint32_t vertex_offset = 0) const;

    inline const Array<VertexAttribute>& attributes() const { return _attributes; }
    inline const Interval3f& bounds() const { return _bounds; }

    static const Mesh& quad();
    static const Mesh& wire_cube();
    static const Mesh& wire_sphere();

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.vertices <= v.indices <= v.attributes; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.vertices >= v.indices >= v.attributes; }
  };
}
