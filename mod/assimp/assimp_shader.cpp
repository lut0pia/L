#include "assimp.h"

using namespace L;

static const Symbol assimp_symbol("assimp"), frag_symbol("frag"), vert_symbol("vert");

bool assimp_shader_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != assimp_symbol) {
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
