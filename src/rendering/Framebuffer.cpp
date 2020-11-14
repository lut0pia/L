#include "Framebuffer.h"

#include "Texture.h"
#include "../stream/CFileStream.h"

using namespace L;

Framebuffer::Framebuffer(uint32_t width, uint32_t height, const RenderPass& render_pass)
  : _width(width), _height(height), _render_pass(render_pass) {
  _impl = Renderer::get()->create_framebuffer(width, height, render_pass);
}
Framebuffer::~Framebuffer() {
  Renderer::get()->destroy_framebuffer(_impl);
}

void Framebuffer::resize(uint32_t width, uint32_t height) {
  this->~Framebuffer();
  new(this)Framebuffer(width, height, _render_pass);
}

void Framebuffer::begin(RenderCommandBuffer* cmd_buffer) {
  Renderer::get()->begin_framebuffer(_impl, cmd_buffer);
}
void Framebuffer::end(RenderCommandBuffer* cmd_buffer) {
  Renderer::get()->end_framebuffer(_impl, cmd_buffer);
}
