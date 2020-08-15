#include "Mesh.h"

using namespace L;

Mesh::Mesh(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t acount, const uint16_t* iarray, size_t icount)
  : Mesh() {
  load(count, data, size, attributes, acount, iarray, icount);
}
Mesh::Mesh(const Intermediate& intermediate) : Mesh() {
  size_t vertex_size = 0;
  for(const VertexAttribute& attribute : intermediate.attributes) {
    vertex_size += Vulkan::format_size(attribute.format);
  }
  load(
    intermediate.vertices.size() / vertex_size,
    intermediate.vertices,
    intermediate.vertices.size(),
    intermediate.attributes.begin(),
    intermediate.attributes.size(),
    (const uint16_t*)(const void*)intermediate.indices,
    intermediate.indices.size() / 2);
}
Mesh::~Mesh() {
  if(_vertex_buffer) Memory::delete_type(_vertex_buffer);
  if(_index_buffer) Memory::delete_type(_index_buffer);
}

void Mesh::load(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t acount, const uint16_t* iarray, size_t icount) {
  if(_vertex_buffer) {
    Memory::delete_type(_vertex_buffer);
    _vertex_buffer = nullptr;
  }
  if(_index_buffer) {
    Memory::delete_type(_index_buffer);
    _index_buffer = nullptr;
  }
  _attributes = Array<VertexAttribute>(attributes, acount);

  _vertex_buffer = Memory::new_type<GPUBuffer>(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  _vertex_buffer->load(data);
  if(iarray) {
    _index_buffer = Memory::new_type<GPUBuffer>(icount * sizeof(*iarray), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    _index_buffer->load(iarray);
    _count = uint32_t(icount);
  } else {
    _count = uint32_t(count);
  }

  // Compute bounds
  const size_t vertex_size(size / count);
  _bounds = (*(Vector3f*)data);
  for(uintptr_t i(1); i < count; i++)
    _bounds.add(*(Vector3f*)((uint8_t*)data + vertex_size*i));
}
void Mesh::draw(VkCommandBuffer cmd_buffer, uint32_t vertex_count, uint32_t index_offset, uint32_t vertex_offset) const {
  if(_vertex_buffer) {
    const VkBuffer buffers[] {*_vertex_buffer};
    const VkDeviceSize offsets[] {0};
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, buffers, offsets);
    if(vertex_count == 0) {
      vertex_count = _count;
    }
    if(_index_buffer) {
      vkCmdBindIndexBuffer(cmd_buffer, *_index_buffer, 0, VK_INDEX_TYPE_UINT16);
      vkCmdDrawIndexed(cmd_buffer, vertex_count, 1, index_offset, vertex_offset, 0);
    } else {
      vkCmdDraw(cmd_buffer, vertex_count, 1, vertex_offset, 0);
    }
  }
}

const Mesh& Mesh::quad() {
  static const float quad[] = {
    -1,-1,0,
    1,-1,0,
    -1,1,0,
    1,1,0,
  };
  static const VertexAttribute attributes[] {{VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Position}};
  static Mesh mesh(4, quad, sizeof(quad), attributes, L_COUNT_OF(attributes));
  return mesh;
}
const Mesh& Mesh::wire_cube() {
  static const float wireCube[] = {
    // Bottom face
    -1,-1,-1, -1,1,-1,
    -1,-1,-1, 1,-1,-1,
    1,-1,-1,  1,1,-1,
    -1,1,-1,  1,1,-1,
    // Top face
    -1,-1,1, -1,1,1,
    -1,-1,1, 1,-1,1,
    1,-1,1,  1,1,1,
    -1,1,1,  1,1,1,
    // Sides
    -1,-1,-1, -1,-1,1,
    -1,1,-1,  -1,1,1,
    1,-1,-1,  1,-1,1,
    1,1,-1,   1,1,1,
  };
  static const VertexAttribute attributes[] {{VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Position}};
  static Mesh mesh(12 * 2, wireCube, sizeof(wireCube), attributes, L_COUNT_OF(attributes));
  return mesh;
}
const Mesh& Mesh::wire_sphere() {
  static const float d(sqrt(.5f));
  static const float wireSphere[] = {
    // X circle
    0,0,-1, 0,-d,-d, 0,-d,-d, 0,-1,0,
    0,-1,0, 0,-d,d,  0,-d,d,  0,0,1,
    0,0,1,  0,d,d,   0,d,d,   0,1,0,
    0,1,0,  0,d,-d,  0,d,-d,  0,0,-1,
    // Y circle
    0,0,-1, -d,0,-d, -d,0,-d, -1,0,0,
    -1,0,0, -d,0,d,  -d,0,d,  0,0,1,
    0,0,1,  d,0,d,   d,0,d,   1,0,0,
    1,0,0,  d,0,-d,  d,0,-d,  0,0,-1,
    // Z circle
    0,-1,0, -d,-d,0, -d,-d,0, -1,0,0,
    -1,0,0, -d,d,0,  -d,d,0,  0,1,0,
    0,1,0,  d,d,0,   d,d,0,   1,0,0,
    1,0,0, d,-d,0,   d,-d,0,  0,-1,0,
  };
  static const VertexAttribute attributes[] {{VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Position}};
  static Mesh mesh(24 * 2, wireSphere, sizeof(wireSphere), attributes, L_COUNT_OF(attributes));
  return mesh;
}
