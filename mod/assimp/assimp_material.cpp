#include "assimp.h"

using namespace L;

bool assimp_material_loader(ResourceSlot& slot, Material::Intermediate& intermediate) {
  if(!assimp_supported_extensions.find(slot.ext)) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  const aiScene* scene = aiImportFileFromMemory((const char*)buffer.data(), (unsigned int)buffer.size(), assimp_import_flags, slot.ext);
  if(!scene) {
    warning("assimp: %s", aiGetErrorString());
    return false;
  }

  uint32_t material_index = 0;
  slot.parameter("material", material_index);

  const aiMesh* mesh = scene->mMeshes[material_index];
  const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  Vector4f color_factor = Vector4f(1.f, 1.f, 1.f, 1.f);
  float metallic_factor = 0.f;
  float roughness_factor = 0.75f;
  aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&color_factor);
  aiGetMaterialColor(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, (aiColor4D*)&color_factor);
  aiGetMaterialFloat(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &metallic_factor);
  aiGetMaterialFloat(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &roughness_factor);

  aiString color_texture_path;
  const bool has_color_texture =
    !material->GetTexture(aiTextureType_BASE_COLOR, 0, &color_texture_path) ||
    !material->GetTexture(aiTextureType_DIFFUSE, 0, &color_texture_path);

  aiString normal_texture_path;
  const bool has_normal_texture = !material->GetTexture(aiTextureType_NORMALS, 0, &normal_texture_path);

  aiString metal_rough_texture_path;
  const bool has_metal_rough_texture = !material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &metal_rough_texture_path);

  String shader_options;
  String mesh_format = "pn";
  const bool textured = has_color_texture || has_normal_texture || has_metal_rough_texture;

  if(has_color_texture) {
    intermediate.texture("color_texture", slot.path + "?comp=bc1&texture=" + (color_texture_path.C_Str() + 1));
    shader_options += "&color_texture";
  }
  if(has_normal_texture) {
    intermediate.texture("normal_texture", slot.path + "?comp=bc1&texture=" + (normal_texture_path.C_Str() + 1));
    shader_options += "&normal_texture";
    mesh_format += "t"; // Need tangents
  }
  if(has_metal_rough_texture) {
    intermediate.texture("metal_rough_texture", slot.path + "?comp=bc1&texture=" + (metal_rough_texture_path.C_Str() + 1));
    shader_options += "&metal_rough_texture";
  }
  if(textured) {
    mesh_format += "u"; // Need texcoords
  }
  if(mesh->HasVertexColors(0)) {
    mesh_format += "c";
  }
  if(mesh->HasBones()) {
    mesh_format += "jw";
  }

  shader_options += "&fmt=" + mesh_format;
  intermediate.mesh(slot.path + "?fmt=" + mesh_format + "&mesh=" + to_string(material_index));
  intermediate.shader(VK_SHADER_STAGE_FRAGMENT_BIT, ".assimp?stage=frag" + shader_options);
  intermediate.shader(VK_SHADER_STAGE_VERTEX_BIT, ".assimp?stage=vert" + shader_options);
  intermediate.vector("color_factor", color_factor);
  intermediate.vector("metal_rough_factor", Vector4f(
    0.f,
    roughness_factor,
    metallic_factor
  ));

  return true;
}
