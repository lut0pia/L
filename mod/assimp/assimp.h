#pragma once

#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/Quaternion.h>
#include <L/src/rendering/Animation.h>
#include <L/src/rendering/Material.h>
#include <L/src/rendering/Mesh.h>
#include <L/src/rendering/Shader.h>
#include <L/src/rendering/Texture.h>
#include <L/src/script/script.h>

L_PUSH_NO_WARNINGS

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>

L_POP_NO_WARNINGS

extern unsigned int assimp_import_flags;
extern L::Array<L::Symbol> assimp_supported_extensions;

bool assimp_animation_loader(L::ResourceSlot& slot, L::Animation::Intermediate& intermediate);
bool assimp_material_loader(L::ResourceSlot& slot, L::Material::Intermediate& intermediate);
bool assimp_mesh_loader(L::ResourceSlot& slot, L::Mesh::Intermediate& intermediate);
bool assimp_scene_loader(L::ResourceSlot& slot, L::ScriptFunction::Intermediate& intermediate);
bool assimp_skeleton_loader(L::ResourceSlot& slot, L::Skeleton::Intermediate& intermediate);
bool assimp_shader_loader(L::ResourceSlot& slot, L::Shader::Intermediate& intermediate);
bool assimp_texture_loader(L::ResourceSlot& slot, L::Texture::Intermediate& intermediate);
