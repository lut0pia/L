#include "Renderer.h"

#include "../dev/debug.h"
#include "../macros.h"

using namespace L;

Renderer* Renderer::_instance;

Renderer::Renderer() {
  _instance = this;
}

void Renderer::init_render_passes() {
  if(_geometry_pass == nullptr) {
    RenderFormat geometry_formats[] = {
      RenderFormat::R8G8B8A8_UNorm, // Color+Metal
      RenderFormat::R16G16B16A16_UNorm, // Normal+Roughness+Emission
      RenderFormat::D24_UNorm_S8_UInt, // Depth
    };

    _geometry_pass = create_render_pass(geometry_formats, L_COUNT_OF(geometry_formats), false, true);
  }

  if(_light_pass == nullptr) {
    RenderFormat light_formats[] = {
      RenderFormat::R16G16B16A16_SFloat, // Light
      RenderFormat::D24_UNorm_S8_UInt, // Depth
    };

    _light_pass = create_render_pass(light_formats, L_COUNT_OF(light_formats), false, false);
  }

  if(_present_pass == nullptr) {
    RenderFormat present_formats[] = {
      RenderFormat::B8G8R8A8_UNorm, // Final color
    };

    _present_pass = create_render_pass(present_formats, L_COUNT_OF(present_formats), true, false);
  }
}

size_t Renderer::format_size(RenderFormat format) {
  switch(format) {
    case RenderFormat::Undefined:
      return 0;
    case RenderFormat::R8_UNorm:
      return 1;
    case RenderFormat::R8G8B8A8_UNorm:
    case RenderFormat::R32_UInt:
    case RenderFormat::R32_SFloat:
    case RenderFormat::D24_UNorm_S8_UInt:
      return 4;
    case RenderFormat::R16G16B16A16_UNorm:
    case RenderFormat::R16G16B16A16_UInt:
    case RenderFormat::R16G16B16A16_SFloat:
    case RenderFormat::R32G32_SFloat:
      return 8;
    case RenderFormat::R32G32B32_SFloat:
      return 12;
    case RenderFormat::R32G32B32A32_UInt:
    case RenderFormat::R32G32B32A32_SFloat:
      return 16;
    default:
      warning("Vulkan: unknown format %d", format);
      return 0;
  }
}
bool Renderer::is_depth_format(RenderFormat format) {
  switch(format) {
    case RenderFormat::D16_UNorm:
    case RenderFormat::D16_UNorm_S8_UInt:
    case RenderFormat::D32_SFloat:
    case RenderFormat::D24_UNorm_S8_UInt:
    case RenderFormat::D32_SFloat_S8_UInt:
      return true;
    default: return false;
  }
}
bool Renderer::is_block_format(RenderFormat format) {
  switch(format) {
    case RenderFormat::BC1_RGB_UNorm_Block:
    case RenderFormat::BC3_UNorm_Block:
    case RenderFormat::BC4_UNorm_Block:
      return true;
    default: return false;
  }
}
