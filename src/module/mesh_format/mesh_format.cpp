#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Mesh.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

static Symbol fmt_symbol("fmt");

static size_t vertex_size(const Array<VertexAttribute>& attributes) {
  size_t size = 0;
  for(const VertexAttribute& attribute : attributes) {
    size += Vulkan::format_size(attribute.format);
  }
  return size;
}
static const VertexAttribute* find_attr(const Array<VertexAttribute>& attributes, VertexAttributeType type) {
  return attributes.find([&type](const VertexAttribute& attribute) {
    return type == attribute.type;
  });
}
static uintptr_t attr_offset(const Array<VertexAttribute>& attributes, VertexAttributeType type) {
  uintptr_t offset = 0;
  for(const VertexAttribute& attribute : attributes) {
    if(attribute.type == type) {
      return offset;
    } else {
      offset += Vulkan::format_size(attribute.format);
    }
  }
  error("mesh_format: Couldn't find offset of attribute");
  return 0;
}

static void compute_indices(const ResourceSlot& slot, Mesh::Intermediate& intermediate) {
  L_SCOPE_MARKER("Compute indices");

  Table<uint32_t, uint16_t> vert_index;
  Array<uint8_t> vertices;
  Array<uint16_t> indices;

  const size_t vert_size = vertex_size(intermediate.attributes);
  const size_t vert_count = intermediate.vertices.size() / vert_size;

  for(uintptr_t i = 0; i < vert_count; i++) {
    const char* vertex = (const char*)intermediate.vertices.data(i*vert_size);
    const uint32_t vert_hash = fnv1a(vertex, vert_size);
    if(const uint16_t* found_index = vert_index.find(vert_hash)) {
      indices.push(*found_index);
    } else {
      const uintptr_t index_full = vertices.size() / vert_size;
      if(index_full > UINT16_MAX) {
        warning("mesh_format: Couldn't index mesh '%s', too many vertices", slot.id);
        return;
      }
      const uint16_t index = uint16_t(index_full);
      vert_index[vert_hash] = index;
      indices.push(index);
      vertices.size(vertices.size() + vert_size);
      memcpy(vertices.begin() + index*vert_size, vertex, vert_size);
    }
  }

  intermediate.vertices = Buffer(vertices.begin(), vertices.size());
  intermediate.indices = Buffer(indices.begin(), indices.size() * sizeof(uint16_t));
}

static void transfer_vertices(const Mesh::Intermediate& intermediate, const Array<VertexAttribute>& new_attributes, Buffer& new_vertices) {
  L_SCOPE_MARKER("Transfer vertices");

  const size_t old_vertex_size = vertex_size(intermediate.attributes);
  const size_t new_vertex_size = vertex_size(new_attributes);
  const size_t vertex_count = intermediate.vertices.size() / old_vertex_size;

  for(const VertexAttribute& old_attribute : intermediate.attributes) {
    if(const VertexAttribute* new_attribute = find_attr(new_attributes, old_attribute.type)) {
      const size_t attribute_size = Vulkan::format_size(old_attribute.format);
      const uintptr_t old_offset = attr_offset(intermediate.attributes, old_attribute.type);
      const uintptr_t new_offset = attr_offset(new_attributes, new_attribute->type);
      const uint8_t* src = (uint8_t*)intermediate.vertices.data(old_offset);
      uint8_t* dst = (uint8_t*)new_vertices.data(new_offset);

      for(uintptr_t i = 0; i < vertex_count; i++) {
        memcpy(dst, src, attribute_size);
        dst += new_vertex_size;
        src += old_vertex_size;
      }
    }
  }
}

static void compute_normals(const Mesh::Intermediate& intermediate, const Array<VertexAttribute>& new_attributes, Buffer& new_vertices) {
  L_SCOPE_MARKER("Compute normals");

  const uint16_t* indices = (const uint16_t*)intermediate.indices.data();
  const void* normals = new_vertices.data(attr_offset(new_attributes, VertexAttributeType::Normal));
  const void* positions = intermediate.vertices.data(attr_offset(intermediate.attributes, VertexAttributeType::Position));
  const size_t old_vertex_size = vertex_size(intermediate.attributes);
  const size_t new_vertex_size = vertex_size(new_attributes);
  const size_t vertex_count = intermediate.vertices.size() / old_vertex_size;
  const size_t index_count = intermediate.indices.size() / sizeof(uint16_t);

#define NORMAL(i) (*((Vector3f*)((uint8_t*)normals + i*new_vertex_size)))
#define POSITION(i) (*((Vector3f*)((uint8_t*)positions + i*old_vertex_size)))

  // Reset all normals
  for(uintptr_t i = 0; i < vertex_count; i++) {
    NORMAL(i) = Vector3f(0.f, 0.f, 0.f);
  }

  // Iterate over triangles
  for(uintptr_t i = 0; i < index_count; i += 3) {
    // Compute triangle normal
    const Vector3f& a(POSITION(indices[i]));
    Vector3f n((POSITION(indices[i + 1]) - a)
      .cross(POSITION(indices[i + 2]) - a));
    n.normalize();

    // Add to all triangle vertex normals
    NORMAL(indices[i]) += n;
    NORMAL(indices[i + 1]) += n;
    NORMAL(indices[i + 2]) += n;
  }

  // Normalize all normals
  for(uintptr_t i = 0; i < vertex_count; i++) {
    NORMAL(i).normalize();
  }

#undef NORMAL
#undef POSITION
}

static void compute_tangents(const Mesh::Intermediate& intermediate, const Array<VertexAttribute>& new_attributes, Buffer& new_vertices) {
  L_SCOPE_MARKER("Compute tangents");

  const uint16_t* indices = (const uint16_t*)intermediate.indices.data();
  const void* tangents = new_vertices.data(attr_offset(new_attributes, VertexAttributeType::Tangent));
  const void* positions = intermediate.vertices.data(attr_offset(intermediate.attributes, VertexAttributeType::Position));
  const void* texcoords = intermediate.vertices.data(attr_offset(intermediate.attributes, VertexAttributeType::TexCoord));
  const size_t old_vertex_size = vertex_size(intermediate.attributes);
  const size_t new_vertex_size = vertex_size(new_attributes);
  const size_t vertex_count = intermediate.vertices.size() / old_vertex_size;
  const size_t index_count = intermediate.indices.size() / sizeof(uint16_t);

#define TANGENT(i) (*((Vector3f*)((uint8_t*)tangents + i*new_vertex_size)))
#define POSITION(i) (*((Vector3f*)((uint8_t*)positions + i*old_vertex_size)))
#define TEXCOORD(i) (*((Vector2f*)((uint8_t*)texcoords + i*old_vertex_size)))

  // Reset all tangents
  for(uintptr_t i = 0; i < vertex_count; i++) {
    TANGENT(i) = Vector3f(0.f, 0.f, 0.f);
  }

  // Iterate over triangles
  for(uintptr_t i = 0; i < index_count; i += 3) {
    const Vector3f& p0 = POSITION(indices[i + 0]);
    const Vector3f& p1 = POSITION(indices[i + 1]);
    const Vector3f& p2 = POSITION(indices[i + 2]);

    const Vector2f& uv0 = TEXCOORD(indices[i + 0]);
    const Vector2f& uv1 = TEXCOORD(indices[i + 1]);
    const Vector2f& uv2 = TEXCOORD(indices[i + 2]);

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

    TANGENT(indices[i + 0]) += sdir;
    TANGENT(indices[i + 1]) += sdir;
    TANGENT(indices[i + 2]) += sdir;
  }

  // Normalize all tangents
  for(uintptr_t i = 0; i < vertex_count; i++) {
    TANGENT(i).normalize();
  }

#undef TANGENT
#undef POSITION
}

void mesh_format_transformer(const ResourceSlot& slot, Mesh::Intermediate& intermediate) {
  if(!intermediate.indices) {
    compute_indices(slot, intermediate);
  }

  const char* format = slot.parameter(fmt_symbol);
  if(format) {
    Array<VertexAttribute> new_attributes;

    while(*format) {
      switch(*format) {
        case 'p': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Position}); break;
        case 'n': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Normal}); break;
        case 't': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeType::Tangent}); break;
        case 'u': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32_SFLOAT, VertexAttributeType::TexCoord}); break;
        case 'c': new_attributes.push(VertexAttribute {VK_FORMAT_R8G8B8A8_UINT, VertexAttributeType::Color}); break;
        case 'j': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32B32A32_UINT, VertexAttributeType::Joints}); break;
        case 'w': new_attributes.push(VertexAttribute {VK_FORMAT_R32G32B32A32_SFLOAT, VertexAttributeType::Weights}); break;
        default: warning("mesh_format: unrecognized attribute '%c'", *format); break;
      }
      format++;
    }

    const size_t old_vertex_size = vertex_size(intermediate.attributes);
    const size_t new_vertex_size = vertex_size(new_attributes);
    const size_t vertex_count = intermediate.vertices.size() / old_vertex_size;
    Buffer new_vertices(new_vertex_size * vertex_count);

    transfer_vertices(intermediate, new_attributes, new_vertices);

    if(intermediate.indices
      && find_attr(intermediate.attributes, VertexAttributeType::Normal) == nullptr
      && find_attr(new_attributes, VertexAttributeType::Normal) != nullptr) {
      compute_normals(intermediate, new_attributes, new_vertices);
    }

    if(intermediate.indices
      && find_attr(intermediate.attributes, VertexAttributeType::Tangent) == nullptr
      && find_attr(intermediate.attributes, VertexAttributeType::TexCoord) != nullptr
      && find_attr(new_attributes, VertexAttributeType::Tangent) != nullptr) {
      compute_tangents(intermediate, new_attributes, new_vertices);
    }

    swap(intermediate.vertices, new_vertices);
    intermediate.attributes = new_attributes;
  }
}

void mesh_format_module_init() {
  ResourceLoading<Mesh>::add_transformer(mesh_format_transformer);
}
