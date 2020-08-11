#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Mesh.h>
#include <L/src/script/script.h>
#include "assimp.h"

using namespace L;

unsigned int assimp_import_flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_FlipUVs;
Array<Symbol> assimp_supported_extensions = {
  "blend", "fbx", "glb", "gltf",
};

void assimp_module_init() {
  ResourceLoading<Animation>::add_loader(assimp_animation_loader);
  ResourceLoading<Material>::add_loader(assimp_material_loader);
  ResourceLoading<Mesh>::add_loader(assimp_mesh_loader);
  ResourceLoading<ScriptFunction>::add_loader(assimp_scene_loader);
  ResourceLoading<Skeleton>::add_loader(assimp_skeleton_loader);
  ResourceLoading<Shader>::add_loader(assimp_shader_loader);
  ResourceLoading<Texture>::add_loader(assimp_texture_loader);
}
