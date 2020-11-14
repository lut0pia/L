#include "assimp.h"

using namespace L;

bool assimp_mesh_loader(ResourceSlot& slot, Mesh::Intermediate& intermediate) {
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

  if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
    warning("assimp: '%s' contains non-triangles");
    return false;
  }

  size_t vertex_size = 0;
  uintptr_t position_offset = 0;
  uintptr_t normal_offset = 0;
  uintptr_t tangent_offset = 0;
  uintptr_t texcoord_offset = 0;
  uintptr_t color_offset = 0;
  uintptr_t joints_offset = 0;
  uintptr_t weights_offset = 0;

  if(mesh->HasPositions()) {
    position_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32_SFloat, VertexAttributeType::Position});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  if(mesh->HasNormals()) {
    normal_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32_SFloat, VertexAttributeType::Normal});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  if(mesh->HasTangentsAndBitangents()) {
    tangent_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32_SFloat, VertexAttributeType::Tangent});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  if(mesh->HasTextureCoords(0)) {
    texcoord_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32_SFloat, VertexAttributeType::TexCoord});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  if(mesh->HasVertexColors(0)) {
    color_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32A32_SFloat, VertexAttributeType::Color});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  if(mesh->HasBones()) {
    joints_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R16G16B16A16_UInt, VertexAttributeType::Joints});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);

    weights_offset = vertex_size;
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32A32_SFloat, VertexAttributeType::Weights});
    vertex_size += Renderer::format_size(intermediate.attributes.back().format);
  }

  intermediate.vertices = Buffer(mesh->mNumVertices * vertex_size);

  for(uintptr_t i = 0; i < mesh->mNumVertices; i++) {
    if(mesh->HasPositions()) {
      Vector3f* dst = (Vector3f*)intermediate.vertices.data(i * vertex_size + position_offset);
      memcpy(dst, mesh->mVertices + i, sizeof(Vector3f));
    }
    if(mesh->HasNormals()) {
      Vector3f* dst = (Vector3f*)intermediate.vertices.data(i * vertex_size + normal_offset);
      memcpy(dst, mesh->mNormals + i, sizeof(Vector3f));
    }
    if(mesh->HasTangentsAndBitangents()) {
      memcpy(intermediate.vertices.data(i * vertex_size + tangent_offset), mesh->mTangents + i, sizeof(Vector3f));
    }
    if(mesh->HasTextureCoords(0)) {
      memcpy(intermediate.vertices.data(i * vertex_size + texcoord_offset), mesh->mTextureCoords[0] + i, sizeof(Vector2f));
    }
    if(mesh->HasVertexColors(0)) {
      memcpy(intermediate.vertices.data(i * vertex_size + color_offset), mesh->mColors[0] + i, sizeof(Vector4f));
    }
    if(mesh->HasBones()) { // Init weight data with zeroes
      memset(intermediate.vertices.data(i * vertex_size + weights_offset), 0, sizeof(Vector4f));
    }
  }

  // Gather influences from all joints
  for(uintptr_t i = 0; i < mesh->mNumBones; i++) {
    const aiBone* bone = mesh->mBones[i];
    for(uintptr_t j = 0; j < bone->mNumWeights; j++) {
      const aiVertexWeight& weight = bone->mWeights[j];
      Vector4f& weights = *(Vector4f*)intermediate.vertices.data(weight.mVertexId * vertex_size + weights_offset);
      for(uintptr_t k = 0; k < 4; k++) {
        if(weights[k] == 0.f) {
          weights[k] = weight.mWeight;
          uint16_t& joint_index = *(uint16_t*)intermediate.vertices.data(weight.mVertexId * vertex_size + joints_offset + k * sizeof(uint16_t));
          joint_index = uint16_t(i);
          break;
        }
      }
    }
  }

  if(mesh->mNumFaces > 0) {
    intermediate.indices = Buffer(mesh->mNumFaces * 3 * sizeof(uint16_t));
    for(uintptr_t i = 0; i < mesh->mNumFaces; i++) {
      for(uintptr_t j = 0; j < 3; j++) {
        *(uint16_t*)intermediate.indices.data((i * 3 + j) * sizeof(uint16_t)) = uint16_t(mesh->mFaces[i].mIndices[j]);
      }
    }
  }

  return true;
}
