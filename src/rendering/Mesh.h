#pragma once

#include "GPUBuffer.h"
#include "../macros.h"
#include "../math/Interval.h"
#include "MeshBuilder.h"

namespace L {
  class Mesh {
    L_NOCOPY(Mesh)
  private:
    GPUBuffer *_vertex_buffer, *_index_buffer;
    Array<VkFormat> _formats;
    Interval3f _bounds;
    uint32_t _count;

  public:
    struct Intermediate {
      MeshBuilder builder;
      Array<VkFormat> formats;
    };
    inline Mesh() : _vertex_buffer(nullptr), _index_buffer(nullptr) {}
    inline Mesh(const Intermediate& intermediate) : Mesh(intermediate.builder, intermediate.formats.begin(), intermediate.formats.size()) {}
    Mesh(size_t count, const void* data, size_t size, const VkFormat* formats, size_t fcount, const uint16_t* indices = nullptr, size_t icount = 0);
    Mesh(const MeshBuilder&, const VkFormat* formats, size_t fcount);
    ~Mesh();
    void load(size_t count, const void* data, size_t size, const VkFormat* formats, size_t fcount, const uint16_t* indices = nullptr, size_t icount = 0);
    void load(const MeshBuilder&, const VkFormat* formats, size_t fcount);
    void draw(VkCommandBuffer) const;

    inline const Interval3f& bounds() const { return _bounds; }

    static const Mesh& quad();
    static const Mesh& wire_cube();
    static const Mesh& wire_sphere();

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.builder <= v.formats; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.builder >= v.formats; }
  };
}
