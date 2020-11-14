#include "Texture.h"

using namespace L;

Texture::Texture(uint32_t width, uint32_t height, RenderFormat format, const void* data, size_t size)
  : _width(width), _height(height), _format(format) {
  _impl = Renderer::get()->create_texture(width, height, format, data, size);
}
Texture::~Texture() {
  Renderer::get()->destroy_texture(_impl);
}

void Texture::load(const void* data, size_t size, Vector3i offset, Vector3i extent) {
  Renderer::get()->load_texture(_impl, data, size, offset, extent);
}

const Texture& Texture::black() {
  static const uint32_t black_color(0);
  static Texture texture(1, 1, RenderFormat::R8G8B8A8_UNorm, &black_color, sizeof(black_color));
  return texture;
}
