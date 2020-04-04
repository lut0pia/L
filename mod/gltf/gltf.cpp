#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/geometry.h>
#include <L/src/math/Quaternion.h>
#include <L/src/rendering/Animation.h>
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

template <class T> static T read_accessor(const cgltf_accessor* acc, intptr_t i = 0) {
  T v;
  read_accessor(acc, i, &v, sizeof(v));
  return v;
}

static Matrix44f gltf_to_l = {
  -1.f, 0.f, 0.f, 0.f,
  0.f,  0.f, 1.f, 0.f,
  0.f,  1.f, 0.f, 0.f,
  0.f,  0.f, 0.f, 1.f,
};

static bool gltf_load_data(ResourceSlot& slot, const Buffer& source, cgltf_data*& data) {
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

static void gltf_compute_node_matrix(const cgltf_node* node, Matrix44f& matrix) {
  if(node->parent) {
    gltf_compute_node_matrix(node->parent, matrix);
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

static const cgltf_node* gltf_root_node(const cgltf_data* data) {
  return data->skins->joints[0]->parent;
}
static Matrix44f gltf_node_matrix(const cgltf_node* node) {
  Matrix44f node_matrix = gltf_to_l;
  gltf_compute_node_matrix(node, node_matrix);
  return node_matrix;
}

static uintptr_t gltf_find_skeleton_joint_index(const cgltf_data* data, const cgltf_node* node) {
  const cgltf_skin* skin = data->skins;
  for(uintptr_t i = 0; i < skin->joints_count; i++) {
    if(skin->joints[i] == node) {
      return i;
    }
  }
  return UINTPTR_MAX;
}
static bool gltf_animation_loader(ResourceSlot& slot, Animation::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  Symbol animation_name = slot.parameter("animation");

  const cgltf_animation* animation = nullptr;

  for(uintptr_t i = 0; i < data->animations_count; i++) {
    if((data->animations[i].name && Symbol(data->animations[i].name) == animation_name)
      || (!data->animations[i].name && !animation_name)) {
      animation = data->animations + i;
      break;
    }
  }

  if(animation == nullptr) {
    warning("GLTF: Could not find animation %s in %s", animation_name, slot.path);
    return false;
  }

  const cgltf_node* root_node = gltf_root_node(data);
  const Matrix44f node_matrix = gltf_node_matrix(root_node);
  const Quatf node_quat = mat_to_quat(node_matrix);

  for(uintptr_t i = 0; i < animation->channels_count; i++) {
    const cgltf_animation_channel& channel = animation->channels[i];
    const cgltf_animation_sampler* sampler = channel.sampler;
    const bool root_joint = channel.target_node->parent == root_node;
    L_ASSERT(sampler->input->component_type == cgltf_component_type_r_32f);
    L_ASSERT(sampler->output->component_type == cgltf_component_type_r_32f);

    intermediate.channels.push();
    AnimationChannel& new_channel = intermediate.channels.back();
    new_channel.joint_index = gltf_find_skeleton_joint_index(data, channel.target_node);
    new_channel.joint_name = channel.target_node->name ? Symbol(channel.target_node->name) : Symbol();

    uintptr_t outputs_per_input = 1;
    switch(sampler->interpolation) {
      case cgltf_interpolation_type_linear:
        new_channel.interpolation = AnimationInterpolationType::Linear;
        break;
      case cgltf_interpolation_type_step:
        new_channel.interpolation = AnimationInterpolationType::Step;
        break;
      case cgltf_interpolation_type_cubic_spline:
        new_channel.interpolation = AnimationInterpolationType::CubicSpline;
        outputs_per_input = 3;
        break;
      default:
        warning("GLTF: Unhandled interpolation type %d in %s", sampler->interpolation, slot.path);
        return false;
    }

    uintptr_t components_per_output = 3;
    switch(channel.target_path) {
      case cgltf_animation_path_type_translation:
        L_ASSERT(sampler->output->type == cgltf_type_vec3);
        new_channel.type = AnimationChannelType::Translation;
        break;
      case cgltf_animation_path_type_rotation:
        L_ASSERT(sampler->output->type == cgltf_type_vec4);
        new_channel.type = AnimationChannelType::Rotation;
        components_per_output = 4;
        break;
      case cgltf_animation_path_type_scale:
        L_ASSERT(sampler->output->type == cgltf_type_vec3);
        new_channel.type = AnimationChannelType::Scale;
        components_per_output = 1;
        break;
      default:
        warning("GLTF: Unhandled animation path %d in %s", channel.target_path, slot.path);
        return false;
    }

    for(uintptr_t j = 0; j < sampler->input->count; j++) {
      const float input = read_accessor<float>(sampler->input, j);
      new_channel.times.push(input);

      for(uintptr_t k = 0; k < outputs_per_input; k++) {
        Vector4f output = read_accessor<Vector4f>(sampler->output, j * outputs_per_input + k);

        // Transform root joints to account for node transform and L coordinates
        if(root_joint) {
          if(channel.target_path == cgltf_animation_path_type_translation) {
            const Vector4f transformed = node_matrix * Vector4f(output.x(), output.y(), output.z(), 0.f);
            output.x() = transformed.x();
            output.y() = transformed.y();
            output.z() = transformed.z();
          } else if(channel.target_path == cgltf_animation_path_type_rotation) {
            output = node_quat * Quatf(output.x(), output.y(), output.z(), output.w());
          }
        }
        if(channel.target_path == cgltf_animation_path_type_scale) {
          // We only support uniform scale so an average will do
          output.x() = (output.x() + output.y() + output.z()) / 3.f;
        }

        for(uintptr_t l = 0; l < components_per_output; l++) {
          new_channel.values.push(output[l]);
        }
      }
    }
  }

  return true;
}

static bool gltf_skeleton_loader(ResourceSlot& slot, Skeleton::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  uint32_t skin_index = 0;
  slot.parameter("skin", skin_index);

  if(data->skins_count <= skin_index) {
    warning("GLTF: Could not find skin %d in %s", skin_index, slot.path);
    return false;
  }

  const cgltf_node* root_node = gltf_root_node(data);
  const Matrix44f node_matrix = gltf_node_matrix(root_node);
  const cgltf_skin* skin = data->skins + skin_index;

  for(uintptr_t i = 0; i < skin->joints_count; i++) {
    SkeletonJoint joint;
    joint.inv_bind_pose = read_accessor<Matrix44f>(skin->inverse_bind_matrices, i);
    joint.inv_bind_pose = joint.inv_bind_pose * node_matrix;
    if(skin->joints[i]->name) {
      joint.name = skin->joints[i]->name;
    }
    joint.parent = -1;
    intermediate.joints.push(joint);
  }

  // Recreate index hierarchy
  for(uintptr_t i = 0; i < skin->joints_count; i++) {
    if(skin->joints[i]->parent) {
      for(uintptr_t j = 0; j < skin->joints_count; j++) {
        if(skin->joints[j] == skin->joints[i]->parent) {
          intermediate.joints[i].parent = j;
          break;
        }
      }
    }
  }

  return true;
}

static bool gltf_material_loader(ResourceSlot& slot, Material::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  cgltf_data* data;
  const Buffer source = slot.read_source_file();
  if(!gltf_load_data(slot, source, data)) {
    return false;
  }

  uint32_t material_index = 0;
  if(slot.parameter("material", material_index)) {
    if(data->materials_count <= material_index) {
      warning("GLTF: Could not find material %d in %s", material_index, slot.path);
      return false;
    }
  }

  const cgltf_material* material = data->materials + material_index;

  static cgltf_material default_material = {};
  if(material == nullptr) {
    default_material.pbr_metallic_roughness.base_color_factor[0] = 1.f;
    default_material.pbr_metallic_roughness.base_color_factor[1] = 1.f;
    default_material.pbr_metallic_roughness.base_color_factor[2] = 1.f;
    default_material.pbr_metallic_roughness.base_color_factor[3] = 1.f;
    material = &default_material;
  }

  // Find mesh
  const cgltf_mesh* mesh = data->meshes;
  for(uintptr_t i = 0; i < data->meshes_count; i++) {
    if(data->meshes[i].primitives_count == 1 && data->meshes[i].primitives->material == material) {
      mesh = data->meshes + i;
      break;
    }
  }

  if(mesh == nullptr) {
    warning("GLTF: Could not find mesh node with material %s in %s", material->name, slot.path);
    return false;
  }

  // Find node
  const cgltf_node* node = data->nodes;
  uintptr_t node_index;
  for(uintptr_t i = 0; i < data->nodes_count; i++) {
    node = data->nodes + i;
    if(node->mesh == mesh) {
      node_index = i;
      break;
    }
  }

  String shader_options;
  String mesh_format = "pn";
  bool textured = false;

#define TEXTURE(name, texpath) \
if(cgltf_texture* texture = texpath.texture) { \
  const uintptr_t texture_index = texture - data->textures; \
  intermediate.texture(name, String(slot.path) + "?comp=bc1&texture=" + to_string(texture_index)); \
  shader_options += "&" name; \
  textured = true; \
}

  TEXTURE("color_texture", material->pbr_metallic_roughness.base_color_texture);
  TEXTURE("normal_texture", material->normal_texture);
  TEXTURE("metal_rough_texture", material->pbr_metallic_roughness.metallic_roughness_texture);

  if(material->normal_texture.texture) {
    mesh_format += "t"; // Need tangents
  }
  if(textured) {
    mesh_format += "u"; // Need texcoords
  }
  for(uintptr_t i = 0; i < node->mesh->primitives->attributes_count; i++) {
    if(node->mesh->primitives->attributes[i].type == cgltf_attribute_type_color) {
      mesh_format += "c";
      break;
    }
  }
  if(node->skin) {
    mesh_format += "jw";
  }

  shader_options += "&fmt=" + mesh_format;
  intermediate.mesh(String(slot.path) + "?fmt=" + mesh_format + "&node=" + to_string(node_index));
  intermediate.shader(VK_SHADER_STAGE_FRAGMENT_BIT, ".glb?stage=frag" + shader_options);
  intermediate.shader(VK_SHADER_STAGE_VERTEX_BIT, ".glb?stage=vert" + shader_options);
  intermediate.vector("color_factor", Vector4f(material->pbr_metallic_roughness.base_color_factor));
  intermediate.vector("metal_rough_factor", Vector4f(
    0.f,
    material->pbr_metallic_roughness.roughness_factor,
    material->pbr_metallic_roughness.metallic_factor
  ));
  return true;
}

static bool gltf_mesh_loader(ResourceSlot& slot, Mesh::Intermediate& intermediate) {
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

  if(primitive->indices) {
    const cgltf_accessor* ind_acc = primitive->indices;

    if(ind_acc->component_type == cgltf_component_type_r_16u) {
      intermediate.indices = Buffer(ind_acc->count * sizeof(uint16_t));
      read_accessor(ind_acc, 0, intermediate.indices.data(), intermediate.indices.size());
    } else {
      warning("GLTF: No support for non-uint16 indices");
      return false;
    }
  }

  const Matrix44f node_matrix = gltf_node_matrix(node);

  size_t vertex_size = 0;

  for(uintptr_t i = 0; i < primitive->attributes_count; i++) {
    const cgltf_attribute& attribute = primitive->attributes[i];
    VertexAttribute mesh_attribute;
    switch(attribute.type) {
      case cgltf_attribute_type_position: mesh_attribute.type = VertexAttributeType::Position; break;
      case cgltf_attribute_type_normal: mesh_attribute.type = VertexAttributeType::Normal; break;
      case cgltf_attribute_type_tangent: mesh_attribute.type = VertexAttributeType::Tangent; break;
      case cgltf_attribute_type_texcoord: mesh_attribute.type = VertexAttributeType::TexCoord; break;
      case cgltf_attribute_type_color: mesh_attribute.type = VertexAttributeType::Color; break;
      case cgltf_attribute_type_joints: mesh_attribute.type = VertexAttributeType::Joints; break;
      case cgltf_attribute_type_weights: mesh_attribute.type = VertexAttributeType::Weights; break;
      default: warning("GLTF: Unknown attribute type in '%s'", slot.id); return false;
    }
    switch(attribute.data->type) {
      case cgltf_type_vec2:
        switch(attribute.data->component_type) {
          case cgltf_component_type_r_32f: mesh_attribute.format = VK_FORMAT_R32G32_SFLOAT; break;
          default: warning("GLTF: Unknown attribute format in '%s'", slot.id); return false;
        }
        break;
      case cgltf_type_vec3:
        switch(attribute.data->component_type) {
          case cgltf_component_type_r_32f: mesh_attribute.format = VK_FORMAT_R32G32B32_SFLOAT; break;
          default: warning("GLTF: Unknown attribute format in '%s'", slot.id); return false;
        }
        break;
      case cgltf_type_vec4:
        switch(attribute.data->component_type) {
          case cgltf_component_type_r_16u: mesh_attribute.format = VK_FORMAT_R16G16B16A16_UINT; break;
          case cgltf_component_type_r_32u: mesh_attribute.format = VK_FORMAT_R32G32B32A32_UINT; break;
          case cgltf_component_type_r_32f: mesh_attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
          default: warning("GLTF: Unknown attribute format in '%s'", slot.id); return false;
        }
        break;
      default: warning("GLTF: Unknown attribute format in '%s'", slot.id); return false;
    }

    intermediate.attributes.push(mesh_attribute);
    vertex_size += attribute.data->stride;
    L_ASSERT(attribute.data->stride == Vulkan::format_size(mesh_attribute.format));
  }

  intermediate.vertices = Buffer(primitive->attributes->data->count * vertex_size);

  size_t offset = 0;
  for(uintptr_t i = 0; i < primitive->attributes_count; i++) {
    const cgltf_attribute& attribute = primitive->attributes[i];
    for(uintptr_t j = 0; j < attribute.data->count; j++) {
      void* dst = intermediate.vertices.data(j * vertex_size + offset);
      read_accessor(attribute.data, j, dst, attribute.data->stride);

      if(attribute.type == cgltf_attribute_type_position) {
        *(Vector3f*)dst = Vector4f(node_matrix * Vector4f(*(Vector3f*)dst, 1.f));
      } else if(attribute.type == cgltf_attribute_type_normal
        || attribute.type == cgltf_attribute_type_tangent) {
        *(Vector3f*)dst = Vector4f(node_matrix * Vector4f(*(Vector3f*)dst, 0.f));
      }
    }
    offset += attribute.data->stride;
  }

  return true;
}

static const Symbol frag_symbol("frag"), vert_symbol("vert");
static bool gltf_shader_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != glb_symbol) {
    return false;
  }

  String source;
  const bool skinned = strchr(slot.parameter("fmt"), 'j') && strchr(slot.parameter("fmt"), 'w');

  const char* mesh_format = slot.parameter("fmt");
  Array<String> vert_attributes;
  while(*mesh_format) {
    switch(*mesh_format) {
      case 'p': vert_attributes.push("vec3 vposition"); break;
      case 'n': vert_attributes.push("vec3 vnormal"); break;
      case 't': vert_attributes.push("vec3 vtangent"); break;
      case 'u': vert_attributes.push("vec2 vtexcoords"); break;
      case 'c': vert_attributes.push("vec4 vcolor"); break;
      case 'j': vert_attributes.push("uvec4 vjoints"); break;
      case 'w': vert_attributes.push("vec4 vweights"); break;
    }
    mesh_format++;
  }

  mesh_format = slot.parameter("fmt");
  Array<String> frag_attributes;
  while(*mesh_format) {
    switch(*mesh_format) {
      case 'n': frag_attributes.push("vec3 fnormal"); break;
      case 't': frag_attributes.push("vec3 ftangent"); break;
      case 'u': frag_attributes.push("vec2 ftexcoords"); break;
      case 'c': frag_attributes.push("vec4 fcolor"); break;
    }
    mesh_format++;
  }

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

    source += "vec4 color = vec4(1,1,1,1);\n";
    if(slot.parameter("color_texture")) {
      source += "color *= texture(color_texture, ftexcoords);\n";
    }
    source += "color.rgb = linearize(color.rgb);\n";
    if(strchr(slot.parameter("fmt"), 'c')) {
      source += "color *= fcolor;\n";
    }
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

    if(skinned) {
      source += "layout(binding = 2) uniform Pose {\n\
        mat4 joints[1024];\n\
      };\n";
    }

    source += "void main() {\n";

    source += "vec4 position = vec4(vposition, 1.f);\n";
    source += "vec4 normal = vec4(vnormal, 0.f);\n";
    if(strchr(slot.parameter("fmt"), 't')) {
      source += "vec4 tangent = vec4(vtangent, 0.f);\n";
    }
    if(skinned) {
#define SKIN(vector) \
      source += vector " = " \
        "(joints[vjoints[0]] * " vector ") * vweights[0]" \
        "+ (joints[vjoints[1]] * " vector ") * vweights[1]" \
        "+ (joints[vjoints[2]] * " vector ") * vweights[2]" \
        "+ (joints[vjoints[3]] * " vector ") * vweights[3];\n"
      SKIN("position");
      SKIN("normal");
      if(strchr(slot.parameter("fmt"), 't')) {
        SKIN("tangent");
      }
    }
    source += "fnormal = normalize((model * normal).xyz);\n";
    if(strchr(slot.parameter("fmt"), 't')) {
      source += "ftangent = normalize((model * tangent).xyz);\n";
    }
    if(strchr(slot.parameter("fmt"), 'u')) {
      source += "ftexcoords = vtexcoords;\n";
    }
    if(strchr(slot.parameter("fmt"), 'c')) {
      source += "fcolor = vcolor;\n";
    }
    source += "gl_Position = viewProj * model * position;\n";
    source += "}\n";
  } else {
    return false;
  }
  slot.source_buffer = Buffer(source.begin(), source.size());

  return ResourceLoading<Shader>::load_internal(slot, intermediate);
}

static bool gltf_texture_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
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
  ResourceLoading<Animation>::add_loader(gltf_animation_loader);
  ResourceLoading<Skeleton>::add_loader(gltf_skeleton_loader);
  ResourceLoading<Material>::add_loader(gltf_material_loader);
  ResourceLoading<Mesh>::add_loader(gltf_mesh_loader);
  ResourceLoading<Shader>::add_loader(gltf_shader_loader);
  ResourceLoading<Texture>::add_loader(gltf_texture_loader);
}
