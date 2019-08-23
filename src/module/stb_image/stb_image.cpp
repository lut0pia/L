#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

L_PUSH_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#define STB_DXT_IMPLEMENTATION
#define STB_DXT_STATIC
#include <stb_dxt.h>

L_POP_NO_WARNINGS

using namespace L;

static Symbol bc1_symbol("bc1"), bc3_symbol("bc3");

bool stb_image_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  int width, height, components;
  uint8_t* img;
  {
    Buffer buffer(slot.read_source_file());
    L_SCOPE_MARKER("stbi_load_from_memory");
    img = stbi_load_from_memory((const stbi_uc*)buffer.data(), int(buffer.size()), &width, &height, &components, 4);
  }
  if(img) {
    intermediate.width = width;
    intermediate.height = height;

    const Symbol compression(slot.parameter("comp"));
    if(compression==bc1_symbol || compression==bc3_symbol) {
      const bool alpha(compression==bc3_symbol);
      const uint32_t block_size_bytes(alpha?16:8);
      const uint32_t block_size_pixels(16);

      if((width%4)==0 && (height%4)==0) {
        intermediate.binary = Buffer((width*height*block_size_bytes)/block_size_bytes);
        intermediate.format = (compression==bc1_symbol) ? VK_FORMAT_BC1_RGB_UNORM_BLOCK : VK_FORMAT_BC3_UNORM_BLOCK;
        const uint32_t width_block(width/4), height_block(height/4);

        L_SCOPE_MARKER("Compression");
        for(uint32_t x(0); x<width_block; x++) {
          for(uint32_t y(0); y<height_block; y++) {
            uint8_t src_block[block_size_pixels*4];
            const uint8_t* src(img+(x*4+y*width*4)*4);
            for(uint32_t row(0); row<4; row++) {
              memcpy(src_block+row*4*4, src+row*width*4, 4*4);
            }
            stb_compress_dxt_block((uint8_t*)intermediate.binary.data()+(x+y*width_block)*block_size_bytes, src_block, alpha, STB_DXT_HIGHQUAL);
          }
        }
      } else warning("Attempting BC compression on texture with incompatible dimensions: %s is %dx%d, dimensions should be multiples of 4.", (const char*)slot.id, width, height);
    }
    if(!intermediate.binary) {
      intermediate.format = VK_FORMAT_R8G8B8A8_UNORM;
      intermediate.binary = Buffer(img, width*height*4);
    }
    stbi_image_free(img);
    return true;
  } else {
    out << stbi_failure_reason() << '\n';
    return false;
  }
}

void stb_image_module_init() {
  ResourceLoading<Texture>::add_loader("jpeg", stb_image_loader);
  ResourceLoading<Texture>::add_loader("jpg", stb_image_loader);
  ResourceLoading<Texture>::add_loader("png", stb_image_loader);
  ResourceLoading<Texture>::add_loader("bmp", stb_image_loader);
}
