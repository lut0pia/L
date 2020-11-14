#include "RenderPass.h"

using namespace L;

RenderPass::RenderPass(const Array<RenderFormat>& formats, bool present) : _formats(formats) {
  const size_t format_count(formats.size());
  _has_depth = false;
  for(uintptr_t i = 0; i < format_count; i++) {
    if(Renderer::is_depth_format(formats[i])) {
      L_ASSERT(i == format_count - 1);
      _has_depth = true;
    }
  }

  _impl = Renderer::get()->create_render_pass(formats.begin(), formats.size(), present);
}
RenderPass::~RenderPass() {
  Renderer::get()->destroy_render_pass(_impl);
}

const RenderPass& RenderPass::geometry_pass() {
  static RenderPass render_pass(Array<RenderFormat> {
    RenderFormat::B8G8R8A8_UNorm, // Color+Metal
      RenderFormat::R16G16B16A16_UNorm, // Normal+Roughness+Emission
      RenderFormat::D24_UNorm_S8_UInt, // Depth
  });
  return render_pass;
}
const RenderPass& RenderPass::light_pass() {
  static RenderPass render_pass(Array<RenderFormat> {
    RenderFormat::R16G16B16A16_SFloat, // Light
  });
  return render_pass;
}
const RenderPass& RenderPass::present_pass() {
  static RenderPass render_pass(Array<RenderFormat> {
    RenderFormat::B8G8R8A8_UNorm, // Final color
  }, true);
  return render_pass;
}
