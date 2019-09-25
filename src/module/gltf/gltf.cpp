#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/Quaternion.h>
#include <L/src/pipeline/ShaderTools.h>
#include <L/src/pipeline/MeshBuilder.h>
#include <L/src/rendering/Material.h>
#include <L/src/rendering/Mesh.h>
#include <L/src/rendering/Shader.h>
#include <L/src/rendering/shader_lib.h>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

L_PUSH_NO_WARNINGS

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

L_POP_NO_WARNINGS

using namespace L;

static const Symbol glb_symbol("glb");

static void read_accessor(const cgltf_data* data, const cgltf_accessor* acc, intptr_t i, void* dst, size_t size) {
  const uint8_t* src = (const uint8_t*)acc->buffer_view->buffer->data;
  const uintptr_t offset = acc->offset + acc->buffer_view->offset + acc->stride * i;
  memcpy(dst, src + offset, size);
}

template <class T> T read_accessor(const cgltf_data* data, const cgltf_accessor* acc, intptr_t i) {
  T v;
  read_accessor(data, acc, i, &v, sizeof(v));
  return v;
}

bool gltf_load_data(ResourceSlot& slot, const Buffer& source, cgltf_data*& data) {
  cgltf_result result;
  {
    L_SCOPE_MARKER("cgltf load");
    cgltf_options options {};
    result = cgltf_parse(&options, source.data(), source.size(), &data);
    if(result == cgltf_result_success) {
      result = cgltf_load_buffers(&options, data, slot.path);
    }
  }

  if(result != cgltf_result_success) {
    warning("cgltf couldn't load file");
  }

  return result == cgltf_result_success;
}

bool gltf_material_loader(ResourceSlot& slot, Material::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  uint32_t material_index = 0;
  slot.parameter("material", material_index);

  if(data->materials_count <= material_index) {
    warning("GLTF: Could not find material %d in %s", material_index, slot.path);
    return false;
  }

  const cgltf_material& material = data->materials[material_index];

  if(!material.pbr_metallic_roughness.base_color_texture.texture
    || !material.normal_texture.texture
    || !material.pbr_metallic_roughness.metallic_roughness_texture.texture) {
    warning("GLTF: Could not find all textures in %s", slot.path);
    return false;
  }

  const uintptr_t base_color_texture = material.pbr_metallic_roughness.base_color_texture.texture - data->textures;
  const uintptr_t normal_texture = material.normal_texture.texture - data->textures;
  const uintptr_t metal_rough_texture = material.pbr_metallic_roughness.metallic_roughness_texture.texture - data->textures;

  intermediate.mesh(String(slot.path) + "?node=0");
  intermediate.shader(VK_SHADER_STAGE_FRAGMENT_BIT, ".glb?stage=frag");
  intermediate.shader(VK_SHADER_STAGE_VERTEX_BIT, ".glb?stage=vert");
  intermediate.texture("base_color_texture", String(slot.path) + "?comp=bc1&texture=" + to_string(base_color_texture));
  intermediate.texture("normal_texture", String(slot.path) + "?comp=bc1&texture=" + to_string(normal_texture));
  intermediate.texture("metal_rough_texture", String(slot.path) + "?comp=bc1&texture=" + to_string(metal_rough_texture));
  return true;
}

inline static void convert_vector(const Quatf& rotation, Vector3f& v) {
  v = rotation.rotate(v);
  
  // GLTF defines +X Left, +Y Up, +Z Forward
  // L defines +X Right, +Y Forward, +Z Up
  const float y = v.y();
  v.x() = -v.x();
  v.y() = v.z();
  v.z() = y;
}

bool gltf_mesh_loader(ResourceSlot& slot, Mesh::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  uint32_t node_index = 0;
  slot.parameter("node", node_index);

  if(data->nodes_count <= node_index) {
    warning("GLTF: Could not find node %d in %s", node_index, slot.path);
    return false;
  }

  const cgltf_node* node = data->nodes + node_index;

  if(node->mesh == nullptr) {
    warning("GLTF: Node %d is not a mesh in %s", node_index, slot.path);
    return false;
  }

  const cgltf_mesh* mesh = node->mesh;

  if(mesh->primitives_count != 1) {
    warning("GLTF: No support for multiple primitives in mesh");
    return false;
  }

  const cgltf_primitive* primitive = mesh->primitives;

  if(primitive->indices == nullptr) {
    warning("GLTF: No support for non-indexed vertices");
    return false;
  }

  const cgltf_accessor* ind_acc = primitive->indices;

  if(ind_acc->component_type != cgltf_component_type_r_16u) {
    warning("GLTF: No support for non-uint16 indices");
    return false;
  }

  cgltf_accessor *pos_acc = nullptr, *nor_acc = nullptr, *tan_acc = nullptr, *tex_acc = nullptr;

  for(uintptr_t i = 0; i < primitive->attributes_count; i++) {
    const cgltf_attribute& attribute = primitive->attributes[i];
    if(!strcmp(attribute.name, "POSITION")) {
      pos_acc = attribute.data;
    } else if(!strcmp(attribute.name, "NORMAL")) {
      nor_acc = attribute.data;
    } else if(!strcmp(attribute.name, "TEXCOORD_0")) {
      tex_acc = attribute.data;
    } else if(!strcmp(attribute.name, "TANGENT")) {
      tan_acc = attribute.data;
    }
  }

  if(pos_acc == nullptr) {
    warning("GLTF: Missing position attribute for mesh %d in %s", node_index, slot.path);
    return false;
  }

  if(tex_acc == nullptr) {
    warning("GLTF: Missing texture coordinates attribute for mesh %d in %s", node_index, slot.path);
    return false;
  }

  struct Vertex {
    Vector3f position, normal, tangent;
    Vector2f uv;
  };

  MeshBuilder mesh_builder(sizeof(Vertex));
  mesh_builder.position_offset = 0;
  mesh_builder.normal_offset = sizeof(Vector3f);
  mesh_builder.tangent_offset = sizeof(Vector3f) * 2;
  mesh_builder.uv_offset = sizeof(Vector3f) * 3;

  Quatf node_rotation;

  if(node->has_rotation) {
    node_rotation = Quatf(node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]);
  }

  for(uintptr_t i(0); i < ind_acc->count; i++) {
    Vertex vertex {};

    const uint16_t index = read_accessor<uint16_t>(data, ind_acc, i);

    vertex.position = read_accessor<Vector3f>(data, pos_acc, index);
    vertex.uv = read_accessor<Vector2f>(data, tex_acc, index);

    if(nor_acc) {
      vertex.normal = read_accessor<Vector3f>(data, nor_acc, index);
    }

    if(tan_acc) {
      vertex.tangent = read_accessor<Vector3f>(data, tan_acc, index);
    }

    // Coordinate system conversion
    convert_vector(node_rotation, vertex.position);
    convert_vector(node_rotation, vertex.normal);
    convert_vector(node_rotation, vertex.tangent);

    mesh_builder.add_vertex(&vertex);
  }

  if(nor_acc == nullptr) {
    mesh_builder.compute_normals();
  }

  if(tan_acc == nullptr) {
    mesh_builder.compute_tangents();
  }

  intermediate.vertices = Buffer(mesh_builder.vertices(), mesh_builder.vertices_size());
  intermediate.indices = Buffer(mesh_builder.indices(), mesh_builder.index_count() * sizeof(uint16_t));
  intermediate.formats = {
    VK_FORMAT_R32G32B32_SFLOAT, // Position
    VK_FORMAT_R32G32B32_SFLOAT, // Normal
    VK_FORMAT_R32G32B32_SFLOAT, // Tangent
    VK_FORMAT_R32G32_SFLOAT, // UV
  };
  return true;
}

bool gltf_shader_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  String source;

  if(slot.parameter("stage") == Symbol("frag")) {
    slot.ext = "frag";
    source =
      "layout(location = 0) in vec3 fposition;\n\
layout(location = 1) in vec3 fnormal;\n\
layout(location = 2) in vec3 ftangent;\n\
layout(location = 3) in vec2 ftexcoords;\n\
\n\
layout(location = 0) out vec4 ocolor;\n\
layout(location = 1) out vec4 onormal;\n\
\n\
layout(binding = 1) uniform sampler2D base_color_texture;\n\
layout(binding = 2) uniform sampler2D normal_texture;\n\
layout(binding = 3) uniform sampler2D metal_rough_texture;\n\
\n\
void main() {\
  vec4 color = texture(base_color_texture, ftexcoords);\n\
  vec3 normal_sample = (texture(normal_texture, ftexcoords).xyz * 2.f) - 1.f;\n\
  vec4 metal_rough = texture(metal_rough_texture, ftexcoords);\n\
  vec3 bitangent = cross(fnormal, ftangent);\n\
  mat3 tangent_space = mat3(ftangent, -bitangent, fnormal);\n\
  vec3 normal = normalize(tangent_space * normal_sample);\n\
\n\
  if(alpha(color.a)) discard;\n\
  ocolor.rgb = linearize(color.rgb);\n\
  ocolor.a = metal_rough.b; // Metalness\n\
  onormal.xy = encodeNormal(normal);\n\
  onormal.z = metal_rough.g; // Roughness\n\
  onormal.w = 0.f; // Emission\n\
}\n";
  } else if(slot.parameter("stage") == Symbol("vert")) {
    slot.ext = "vert";
    source =
      "layout(location = 0) in vec3 vposition;\n\
layout(location = 1) in vec3 vnormal;\n\
layout(location = 2) in vec3 vtangent;\n\
layout(location = 3) in vec2 vtexcoords;\n\
\n\
layout(location = 0) out vec3 fposition;\n\
layout(location = 1) out vec3 fnormal;\n\
layout(location = 2) out vec3 ftangent;\n\
layout(location = 3) out vec2 ftexcoords;\n\
\n\
void main() {\n\
  ftexcoords = vtexcoords;\n\
  fnormal = normalize(mat3(model) * vnormal);\n\
  ftangent = normalize(mat3(model) * vtangent);\n\
  vec4 position = model * vec4(vposition, 1.0);\n\
  fposition = position.xyz;\n\
  gl_Position = viewProj * position;\n\
}\n";
  } else {
    return false;
  }
  slot.source_buffer = Buffer(source.begin(), source.size());
  slot.persistent = true;

  return ResourceLoading<Shader>::load_internal(slot, intermediate);
}

bool gltf_texture_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }
  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  uint32_t texture_index = 0;
  slot.parameter("texture", texture_index);

  if(data->textures_count <= texture_index) {
    warning("GLTF: Could not find texture %d in %s", texture_index, slot.path);
    return false;
  }

  const cgltf_image* image = data->textures[texture_index].image;
  const cgltf_buffer_view* buffer_view = image->buffer_view;

  slot.ext = strrchr(image->mime_type, '/') + 1;
  slot.source_buffer = Buffer((uint8_t*)buffer_view->buffer->data + buffer_view->offset, image->buffer_view->size);

  return ResourceLoading<Texture>::load_internal(slot, intermediate);
}

void gltf_module_init() {
  ResourceLoading<Material>::add_loader(gltf_material_loader);
  ResourceLoading<Mesh>::add_loader(gltf_mesh_loader);
  ResourceLoading<Shader>::add_loader(gltf_shader_loader);
  ResourceLoading<Texture>::add_loader(gltf_texture_loader);
}
