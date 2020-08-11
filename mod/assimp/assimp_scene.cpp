#include "assimp.h"

using namespace L;

static bool assimp_scene_load_node(ResourceSlot& slot, Script& script, const aiNode* node) {
  aiMatrix4x4 transform = node->mTransformation;
  for(const aiNode* parent = node->mParent; parent; parent = parent->mParent) {
    transform = parent->mTransformation * transform;
  }
  
  aiVector3t<ai_real> scaling;
  aiQuaterniont<ai_real> rotation;
  aiVector3t<ai_real> position;
  transform.Decompose(scaling, rotation, position);

  // Create entity
  script.bytecode.push(ScriptInstruction {ScriptOpCode::LoadGlobal, 0});
  script.bytecode.back().bcu16 = script.global("group_entity_create");
  script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 0, 0});

  // Create transform component
  script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 0, uint8_t(script.constant(Symbol("add_transform"))), 1}); // Get method
  script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 2, 0}); // Copy entity as self
  script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 1, 0}); // Call method with no arguments

  // Set transform position
  script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 1, uint8_t(script.constant(Symbol("set_position"))), 2}); // Get method
  script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 3, 1}); // Copy transform as self
  script.bytecode.push(ScriptInstruction {ScriptOpCode::LoadConst, 4}); // Load position vector as first argument
  script.bytecode.back().bcu16 = script.constant(Vector3f(position.x, position.y, position.z));
  script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 2, 1}); // Call method with one argument

  // Set transform rotation
  script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 1, uint8_t(script.constant(Symbol("set_rotation"))), 2}); // Get method
  script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 3, 1}); // Copy transform as self
  script.bytecode.push(ScriptInstruction {ScriptOpCode::LoadConst, 4}); // Load quaternion as first argument
  script.bytecode.back().bcu16 = script.constant(Quatf(rotation.x, rotation.y, rotation.z, rotation.w));
  script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 2, 1}); // Call method with one argument

  for(uintptr_t i = 0; i < node->mNumMeshes; i++) {
    // Create primitive component
    script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 0, uint8_t(script.constant(Symbol("add_primitive"))), 1}); // Get method
    script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 2, 0}); // Copy entity as self
    script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 1, 0}); // Call method with no arguments

    // Get material handle
    script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 1, uint8_t(script.constant(Symbol("material"))), 2}); // Get method
    script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 3, 1}); // Copy primitive as self
    script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 2, 0}); // Call method with no arguments

    // Set material parent
    script.bytecode.push(ScriptInstruction {ScriptOpCode::GetItemConst, 2, uint8_t(script.constant(Symbol("parent"))), 3}); // Get method
    script.bytecode.push(ScriptInstruction {ScriptOpCode::CopyLocal, 4, 2}); // Copy material as self
    script.bytecode.push(ScriptInstruction {ScriptOpCode::LoadConst, 5}); // Load position vector as first argument
    script.bytecode.back().bcu16 = script.constant(slot.path + String("?material=") + to_string(node->mMeshes[i]));
    script.bytecode.push(ScriptInstruction {ScriptOpCode::Call, 3, 1}); // Call method with one argument
  }

  for(uintptr_t i = 0; i < node->mNumChildren; i++) {
    const aiNode* child = node->mChildren[i];
    assimp_scene_load_node(slot, script, child);
  }
  return true;
}
bool assimp_scene_loader(ResourceSlot& slot, ScriptFunction::Intermediate& intermediate) {
  if(!assimp_supported_extensions.find(slot.ext)) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  const aiScene* scene = aiImportFileFromMemory((const char*)buffer.data(), (unsigned int)buffer.size(), assimp_import_flags, slot.ext);
  if(!scene) {
    warning("assimp: %s", aiGetErrorString());
    return false;
  }

  intermediate.script = ref<Script>();

  if(!assimp_scene_load_node(slot, *intermediate.script, scene->mRootNode)) {
    aiReleaseImport(scene);
    return false;
  }

  intermediate.script->bytecode.push(ScriptInstruction {ScriptOpCode::Return});

  aiReleaseImport(scene);

  return true;
}
