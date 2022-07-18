#include "Texture.h"

using namespace L;

Texture::Texture(const Intermediate& intermediate)
  : _width(intermediate.width), _height(intermediate.height), _format(intermediate.format) {
  Array<const void*> mips;
  Array<size_t> sizes;
  for(const Buffer& mip : intermediate.mips) {
    mips.push(mip);
    sizes.push(mip.size());
  }
  _impl = Renderer::get()->create_texture(_width, _height, _format, mips.begin(), sizes.begin(), intermediate.mips.size());
}
Texture::Texture(uint32_t width, uint32_t height, RenderFormat format, const void* data, size_t size)
  : Texture(width, height, format, &data, &size, 1) {}
Texture::Texture(uint32_t width, uint32_t height, RenderFormat format, const void** data, size_t* size, uint32_t mips)
  : _width(width), _height(height), _format(format) {
  _impl = Renderer::get()->create_texture(width, height, format, data, size, mips);
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

size_t L::resource_gpu_size(const Texture::Intermediate& v) {
  size_t gpu_size = 0;
  for(const Buffer& mip : v.mips) {
    gpu_size += mip.size();
  }
  return gpu_size;
}
