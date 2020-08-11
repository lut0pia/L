#include "assimp.h"

using namespace L;

bool assimp_animation_loader(ResourceSlot& slot, Animation::Intermediate& intermediate) {
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

  const aiAnimation* animation;
  if(Symbol anim_name = slot.parameter("name")) {
    animation = nullptr;
    const size_t anim_name_len = strlen(anim_name);
    for(uintptr_t i = 0; i < scene->mNumAnimations; i++) {
      const char* other_anim_name = scene->mAnimations[i]->mName.C_Str();
      const size_t other_anim_name_len = strlen(other_anim_name);
      if(other_anim_name_len >= anim_name_len &&
        !strcmp(other_anim_name + other_anim_name_len - anim_name_len, anim_name)) {
        animation = scene->mAnimations[i];
        break;
      }
    }
    if(animation == nullptr) {
      warning("assimp: Unable to find animation '%s' in '%s'", anim_name, slot.id);
      return false;
    }
  } else if(scene->mNumAnimations > 0) {
    animation = scene->mAnimations[0];
  } else {
    warning("assimp: No animation in '%s'", slot.id);
    return false;
  }

  // I don't know you tell me
  const bool is_gltf = slot.ext == Symbol("glb") || slot.ext == Symbol("gltf");
  const float ticks_per_second = is_gltf ? 1000.f : float(animation->mTicksPerSecond);

  intermediate.duration = float(animation->mDuration) / ticks_per_second;

  for(uintptr_t i = 0; i < animation->mNumChannels; i++) {
    const aiNodeAnim* channel = animation->mChannels[i];
    uintptr_t joint = 0;
    for(uintptr_t j = 0; j < mesh->mNumBones; j++) {
      if(mesh->mBones[j]->mName == channel->mNodeName) {
        joint = j;
        break;
      }
    }

    if(channel->mNumPositionKeys > 0) {
      AnimationChannel trans_chan;
      trans_chan.joint_index = joint;
      trans_chan.joint_name = channel->mNodeName.C_Str();
      trans_chan.type = AnimationChannelType::Translation;
      trans_chan.interpolation = AnimationInterpolationType::Linear;
      for(uintptr_t j = 0; j < channel->mNumPositionKeys; j++) {
        const aiVectorKey& key = channel->mPositionKeys[j];
        trans_chan.times.push(float(key.mTime / ticks_per_second));
        trans_chan.values.push(key.mValue.x);
        trans_chan.values.push(key.mValue.y);
        trans_chan.values.push(key.mValue.z);
      }
      intermediate.channels.push(trans_chan);
    }

    if(channel->mNumRotationKeys > 0) {
      AnimationChannel rot_chan;
      rot_chan.joint_index = joint;
      rot_chan.joint_name = channel->mNodeName.C_Str();
      rot_chan.type = AnimationChannelType::Rotation;
      rot_chan.interpolation = AnimationInterpolationType::Linear;
      for(uintptr_t j = 0; j < channel->mNumRotationKeys; j++) {
        const aiQuatKey& key = channel->mRotationKeys[j];
        rot_chan.times.push(float(key.mTime / ticks_per_second));
        rot_chan.values.push(key.mValue.x);
        rot_chan.values.push(key.mValue.y);
        rot_chan.values.push(key.mValue.z);
        rot_chan.values.push(key.mValue.w);
      }
      intermediate.channels.push(rot_chan);
    }

    if(channel->mNumScalingKeys > 0) {
      AnimationChannel scale_chan;
      scale_chan.joint_index = joint;
      scale_chan.joint_name = channel->mNodeName.C_Str();
      scale_chan.type = AnimationChannelType::Scale;
      scale_chan.interpolation = AnimationInterpolationType::Linear;
      for(uintptr_t j = 0; j < channel->mNumScalingKeys; j++) {
        const aiVectorKey& key = channel->mScalingKeys[j];
        scale_chan.times.push(float(key.mTime / ticks_per_second));
        scale_chan.values.push((key.mValue.x + key.mValue.y + key.mValue.z) / 3.f);
      }
      intermediate.channels.push(scale_chan);
    }
  }

  return true;
}
