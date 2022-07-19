#include <L/src/container/Buffer.h>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

#include <L/src/engine/Resource.inl>

L_PUSH_NO_WARNINGS

#define STB_DXT_IMPLEMENTATION
#define STB_DXT_STATIC
#include <stb_dxt.h>

L_POP_NO_WARNINGS

using namespace L;

static bool uses_alpha(const Texture::Intermediate& intermediate) {
  const uint8_t* pixel_data = (const uint8_t*)intermediate.mips[0].data();
  for(uintptr_t x = 0; x < intermediate.width; x++) {
    for(uintptr_t y = 0; y < intermediate.height; y++) {
      const uint8_t* p = pixel_data + (x + y * intermediate.width) * 4;
      if(p[3] != 0xff) {
        return true;
      }
    }
  }
  return false;
}

static RenderFormat get_destination_format(const Texture::Intermediate& intermediate) {
  if(intermediate.width % 4 == 0 && intermediate.height % 4 == 0) {
    if(intermediate.format == RenderFormat::R8G8B8A8_UNorm) {
      if(uses_alpha(intermediate)) {
        return RenderFormat::BC3_UNorm_Block;
      } else {
        return RenderFormat::BC1_RGB_UNorm_Block;
      }
    } else if(intermediate.format == RenderFormat::R8_UNorm) {
      return RenderFormat::BC4_UNorm_Block;
    }
  }
  return RenderFormat::Undefined;
}

static size_t get_block_size(const RenderFormat& fmt) {
  switch(fmt) {
    case RenderFormat::BC1_RGB_UNorm_Block:
    case RenderFormat::BC4_UNorm_Block:
      return 8;
    case RenderFormat::BC3_UNorm_Block:
      return 16;
    default:
      return 0;
  }
}

void stb_dxt_transformer(const ResourceSlot&, Texture::Intermediate& intermediate) {
  const RenderFormat dest_format = get_destination_format(intermediate);
  if(dest_format != RenderFormat::Undefined) {
    L_SCOPE_MARKER("stb_dxt");

    const bool alpha = uses_alpha(intermediate);
    const uintptr_t block_size_bytes = get_block_size(dest_format);
    const uintptr_t pixel_bytes = Renderer::format_size(intermediate.format);
    const uintptr_t block_size_pixels = 16;
    intermediate.format = dest_format;

    for(uintptr_t mip = 0; mip < intermediate.mips.size(); mip++) {
      const uint32_t mip_width = intermediate.width >> mip;
      const uint32_t mip_height = intermediate.height >> mip;
      if(mip_width % 4 == 0 && mip_height % 4 == 0) {
        Buffer& uncompressed = intermediate.mips[mip];
        Buffer compressed = (mip_width * mip_height * block_size_bytes) / block_size_pixels;
        const uint32_t width_block(mip_width / 4), height_block(mip_height / 4);

        for(uint32_t x(0); x < width_block; x++) {
          for(uint32_t y(0); y < height_block; y++) {
            uint8_t src_block[block_size_pixels * 4];
            const uint8_t* src = (const uint8_t*)uncompressed.data() + (x * 4 + y * mip_width * 4) * pixel_bytes;
            for(uint32_t row = 0; row < 4; row++) {
              memcpy(src_block + row * 4 * pixel_bytes, src + row * mip_width * pixel_bytes, 4 * pixel_bytes);
            }
            uint8_t* dst_block = (uint8_t*)compressed.data() + (x + y * width_block) * block_size_bytes;
            switch(dest_format) {
              case RenderFormat::BC1_RGB_UNorm_Block:
                stb_compress_dxt_block(dst_block, src_block, false, STB_DXT_HIGHQUAL);
                break;
              case RenderFormat::BC3_UNorm_Block:
                stb_compress_dxt_block(dst_block, src_block, true, STB_DXT_HIGHQUAL);
                break;
              case RenderFormat::BC4_UNorm_Block:
                stb_compress_bc4_block(dst_block, src_block);
                break;
            }
          }
        }

        uncompressed = static_cast<Buffer&&>(compressed);
      } else if(mip_width < 4 || mip_height < 4) {
        // TODO: This is not correct but good enough for now
        intermediate.mips[mip] = Buffer(intermediate.mips[mip - 1]);
      }
    }
  }
}

void stb_dxt_module_init() {
  ResourceLoading<Texture>::add_transformer(stb_dxt_transformer, ResourceTransformPhase::VeryLate);
}
