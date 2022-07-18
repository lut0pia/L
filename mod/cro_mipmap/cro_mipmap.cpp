#include <L/src/container/Buffer.h>
#include <L/src/math/math.h>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

#include <L/src/engine/Resource.inl>

L_PUSH_NO_WARNINGS

#include <cro_mipmap.h>

L_POP_NO_WARNINGS

using namespace L;

static Symbol mipmap_symbol("mipmap");

static bool is_format_supported(RenderFormat fmt) {
  return fmt == RenderFormat::R8G8B8A8_UNorm || fmt == RenderFormat::B8G8R8A8_UNorm;
}

void cro_mipmap_transformer(const ResourceSlot&, Texture::Intermediate& intermediate) {
  if(is_format_supported(intermediate.format) &&
     is_pot(intermediate.width) &&
     is_pot(intermediate.height)) {
    uintptr_t current_index = 0;
    uint32_t current_width = intermediate.width;
    uint32_t current_height = intermediate.height;

    uint32_t mip_count = cro_GetMipMapLevels(intermediate.width, intermediate.width);
    for(uint32_t i = 0; i < mip_count; i++) {
      uint32_t next_width, next_height;
      cro_GetMipMapSize(current_width, current_height, &next_width, &next_height);
      intermediate.mips.push(Buffer(next_width * next_height * Renderer::format_size(intermediate.format)));
      const void* current_mip = intermediate.mips[current_index];
      void* next_mip = intermediate.mips[current_index + 1];
      cro_GenMipMapAvgI((const int*)current_mip, current_width, current_height, (int*)next_mip);
      current_width = next_width;
      current_height = next_height;
      current_index++;
    }
  }
}

void cro_mipmap_module_init() {
  ResourceLoading<Texture>::add_transformer(cro_mipmap_transformer);
}
