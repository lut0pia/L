#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/geometry.h>
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

static void read_accessor(const cgltf_accessor* acc, intptr_t i, void* dst, size_t size) {
  const uint8_t* src = (const uint8_t*)acc->buffer_view->buffer->data;
  const uintptr_t offset = acc->offset + acc->buffer_view->offset + acc->stride * i;
  memcpy(dst, src + offset, size);
}

template <class T> T read_accessor(const cgltf_accessor* acc, intptr_t i) {
  T v;
  read_accessor(acc, i, &v, sizeof(v));
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

  // Find mesh node
  const cgltf_mesh* mesh = nullptr;
  uintptr_t node_index;
  for(uintptr_t i = 0; i < data->nodes_count; i++) {
    const cgltf_node* node = data->nodes + i;
    if(node->mesh && node->mesh->primitives_count == 1 && node->mesh->primitives->material == &material) {
      mesh = node->mesh;
      node_index = i;
      break;
    }
  }
  if(mesh) {
    intermediate.mesh(String(slot.path) + "?node=" + to_string(node_index));
  } else {
    warning("GLTF: Could not find mesh node with material %s in %s", material.name, slot.path);
    return false;
  }

  String shader_options;

#define TEXTURE(name, texpath) \
if(cgltf_texture* texture = texpath.texture) { \
  const uintptr_t texture_index = texture - data->textures; \
  intermediate.texture(name, String(slot.path) + "?comp=bc1&texture=" + to_string(texture_index)); \
  shader_options += "&" name; \
}

  TEXTURE("color_texture", material.pbr_metallic_roughness.base_color_texture);
  TEXTURE("normal_texture", material.normal_texture);
  TEXTURE("metal_rough_texture", material.pbr_metallic_roughness.metallic_roughness_texture);

  intermediate.shader(VK_SHADER_STAGE_FRAGMENT_BIT, ".glb?stage=frag" + shader_options);
  intermediate.shader(VK_SHADER_STAGE_VERTEX_BIT, ".glb?stage=vert" + shader_options);
  intermediate.vector("color_factor", Vector4f(material.pbr_metallic_roughness.base_color_factor));
  intermediate.vector("metal_rough_factor", Vector4f(
    0.f,
    material.pbr_metallic_roughness.roughness_factor,
    material.pbr_metallic_roughness.metallic_factor
  ));
  return true;
}

static void compute_node_matrix(const cgltf_node* node, Matrix44f& matrix) {
  if(node->parent) {
    compute_node_matrix(node->parent, matrix);
  }
  if(node->has_rotation) {
    float scale = node->scale[0];
    Vector3f translation = node->translation;
    matrix = matrix * sqt_to_mat(Quatf(node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]), translation, scale);
  }
  if(node->has_matrix) {
    matrix = matrix * Matrix44f(node->matrix);
  }
}
inline static void convert_vector(const Matrix44f& matrix, Vector3f& v) {
  v = Vector4f(matrix * v);

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
    switch(attribute.type) {
      case cgltf_attribute_type_position: pos_acc = attribute.data; break;
      case cgltf_attribute_type_normal: nor_acc = attribute.data; break;
      case cgltf_attribute_type_texcoord: tex_acc = attribute.data; break;
      case cgltf_attribute_type_tangent: tan_acc = attribute.data; break;
      default: break;
    }
  }

  if(pos_acc == nullptr) {
    warning("GLTF: Missing position attribute for mesh %d in %s", node_index, slot.path);
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

  Matrix44f node_matrix(1.f);
  compute_node_matrix(node, node_matrix);

  for(uintptr_t i(0); i < ind_acc->count; i++) {
    Vertex vertex {};

    const uint16_t index = read_accessor<uint16_t>(ind_acc, i);

    vertex.position = read_accessor<Vector3f>(pos_acc, index);
    convert_vector(node_matrix, vertex.position);

    if(tex_acc) {
      vertex.uv = read_accessor<Vector2f>(tex_acc, index);
    }

    if(nor_acc) {
      vertex.normal = read_accessor<Vector3f>(nor_acc, index);
      convert_vector(node_matrix, vertex.normal);
    }

    if(tan_acc) {
      vertex.tangent = read_accessor<Vector3f>(tan_acc, index);
      convert_vector(node_matrix, vertex.tangent);
    }

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
  intermediate.attributes = {
    {VK_FORMAT_R32G32B32_SFLOAT, MeshAttributeType::Position},
    {VK_FORMAT_R32G32B32_SFLOAT, MeshAttributeType::Normal},
    {VK_FORMAT_R32G32B32_SFLOAT, MeshAttributeType::Tangent},
    {VK_FORMAT_R32G32_SFLOAT, MeshAttributeType::TexCoord},
  };
  return true;
}

static const Symbol frag_symbol("frag"), vert_symbol("vert");
bool gltf_shader_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  String source;

  Array<String> vert_attributes;
  vert_attributes.push("vec3 vposition");
  vert_attributes.push("vec3 vnormal");
  vert_attributes.push("vec3 vtangent");
  vert_attributes.push("vec2 vtexcoords");

  Array<String> frag_attributes;
  frag_attributes.push("vec3 fposition");
  frag_attributes.push("vec3 fnormal");
  frag_attributes.push("vec3 ftangent");
  frag_attributes.push("vec2 ftexcoords");

  Array<String> render_targets;
  render_targets.push("vec4 ocolor");
  render_targets.push("vec4 onormal");

  Array<String> samplers;
  if(slot.parameter("color_texture")) {
    samplers.push("sampler2D color_texture");
  }
  if(slot.parameter("normal_texture")) {
    samplers.push("sampler2D normal_texture");
  }
  if(slot.parameter("metal_rough_texture")) {
    samplers.push("sampler2D metal_rough_texture");
  }

  const char* frag_dir = (slot.parameter("stage") == frag_symbol) ? "in" : "out";
  for(uintptr_t i = 0; i < frag_attributes.size(); i++) {
    source += "layout(location = " + to_string(i) + ") " + frag_dir + " " + frag_attributes[i] + ";\n";
  }

  slot.ext = slot.parameter("stage");
  if(slot.ext == frag_symbol) {

    for(uintptr_t i = 0; i < render_targets.size(); i++) {
      source += "layout(location = " + to_string(i) + ") out " + render_targets[i] + ";\n";
    }

    source += "layout(binding = 1) uniform Parameters {\n\
      vec4 color_factor;\n\
      vec4 metal_rough_factor;\n\
    };\n";

    for(uintptr_t i = 0; i < samplers.size(); i++) {
      source += "layout(binding = " + to_string(i + 2) + ") uniform " + samplers[i] + ";\n";
    }

    source += "void main() {\n";

    if(slot.parameter("color_texture")) {
      source += "vec4 color = texture(color_texture, ftexcoords);\n";
    } else {
      source += "vec4 color = vec4(1,1,1,1);\n";
    }
    source += "color.rgb = linearize(color.rgb);\n";
    source += "color *= color_factor;\n";

    if(slot.parameter("normal_texture")) {
      source += "vec3 normal_sample = (texture(normal_texture, ftexcoords).xyz * 2.f) - 1.f;\n";
      source += "vec3 bitangent = cross(fnormal, ftangent);\n";
      source += "mat3 tangent_space = mat3(ftangent, -bitangent, fnormal);\n";
      source += "vec3 normal = normalize(tangent_space * normal_sample);\n";
    } else {
      source += "vec3 normal = fnormal;\n";
    }

    if(slot.parameter("metal_rough_texture")) {
      source += "vec4 metal_rough = texture(metal_rough_texture, ftexcoords);\n";
    } else {
      source += "vec4 metal_rough = vec4(1,1,1,1);\n";
    }
    source += "metal_rough *= metal_rough_factor;\n";

    source += "if(alpha(color.a)) discard;\n";
    source += "ocolor.rgb = linearize(color.rgb);\n";
    source += "ocolor.a = metal_rough.b;\n"; // Metalness
    source += "onormal.xy = encodeNormal(normal);\n";
    source += "onormal.z = metal_rough.g;\n"; // Roughness
    source += "onormal.w = 0.f; // Emission\n";
    source += "}\n";
  } else if(slot.ext == vert_symbol) {

    for(uintptr_t i = 0; i < vert_attributes.size(); i++) {
      source += "layout(location = " + to_string(i) + ") in " + vert_attributes[i] + ";\n";
    }

    source +=
      "void main() {\n\
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
