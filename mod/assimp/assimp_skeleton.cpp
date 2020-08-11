#include "assimp.h"

using namespace L;

bool assimp_skeleton_loader(ResourceSlot& slot, Skeleton::Intermediate& intermediate) {
  if(!assimp_supported_extensions.find(slot.ext)) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  const aiScene* scene = aiImportFileFromMemory((const char*)buffer.data(), (unsigned int)buffer.size(), assimp_import_flags, slot.ext);
  if(!scene) {
    warning("assimp: %s", aiGetErrorString());
    return false;
  }

  uint32_t mesh_index = 0;
  slot.parameter("mesh", mesh_index);

  const aiMesh* mesh = scene->mMeshes[mesh_index];

  for(uintptr_t i = 0; i < mesh->mNumBones; i++) {
    SkeletonJoint joint;
    const aiBone* bone = mesh->mBones[i];
    joint.name = bone->mName.C_Str();
    joint.parent = -1;
    joint.inv_bind_pose = (*(Matrix44f*)(&bone->mOffsetMatrix)).transpose();
    intermediate.joints.push(joint);
  }

  // Recreate index hierarchy
  for(uintptr_t i = 0; i < intermediate.joints.size(); i++) {
    SkeletonJoint& joint = intermediate.joints[i];
    const aiNode* node = scene->mRootNode->FindNode(joint.name);
    for(uintptr_t j = 0; j < intermediate.joints.size(); j++) {
      SkeletonJoint& other_joint = intermediate.joints[j];
      const aiNode* other_node = scene->mRootNode->FindNode(other_joint.name);
      if(node->mParent == other_node) {
        joint.parent = j;
      }
    }
  }

  return true;
}
