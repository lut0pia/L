#include "OpenGLRenderer.h"

using namespace L;

RenderPassImpl* OpenGLRenderer::create_render_pass(const RenderFormat*, size_t, bool) {
  return nullptr;
}
void OpenGLRenderer::destroy_render_pass(RenderPassImpl*) {

}
