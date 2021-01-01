#include "OpenGLRenderer.h"

using namespace L;

RenderPassImpl* OpenGLRenderer::create_render_pass(const RenderFormat*, size_t, bool, bool depth_write) {
  OpenGLRenderPass* render_pass = Memory::new_type<OpenGLRenderPass>();
  render_pass->depth_write = depth_write;
  return render_pass;
}
void OpenGLRenderer::destroy_render_pass(RenderPassImpl* render_pass) {
  Memory::delete_type((OpenGLRenderPass*)render_pass);
}
