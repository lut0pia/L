#include "assimp.h"

using namespace L;

bool assimp_texture_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  if(!assimp_supported_extensions.find(slot.ext)) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  const aiScene* scene = aiImportFileFromMemory((const char*)buffer.data(), (unsigned int)buffer.size(), assimp_import_flags, slot.ext);
  if(!scene) {
    warning("assimp: %s", aiGetErrorString());
    return false;
  }

  uint32_t texture_index = 0;
  slot.parameter("texture", texture_index);

  const aiTexture* texture = scene->mTextures[texture_index];
  if(texture->mHeight == 0) { // Compressed
    slot.ext = texture->achFormatHint;
    slot.source_buffer = Buffer(texture->pcData, texture->mWidth);
    return ResourceLoading<Texture>::load_internal(slot, intermediate);
  } else {
    intermediate.binary = Buffer(texture->pcData, texture->mWidth * texture->mHeight * 4);
    intermediate.width = texture->mWidth;
    intermediate.height = texture->mHeight;
    intermediate.format = VK_FORMAT_R8G8B8A8_UNORM;
  }
  return true;
}
