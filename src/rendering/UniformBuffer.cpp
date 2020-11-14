#include "UniformBuffer.h"

using namespace L;

UniformBuffer::UniformBuffer(size_t size) : _size(size) {
  _impl = Renderer::get()->create_uniform_buffer(size);
}
UniformBuffer::~UniformBuffer() {
  Renderer::get()->destroy_uniform_buffer(_impl);
}

void UniformBuffer::load(const void* src, size_t size, size_t offset) {
  Renderer::get()->load_uniform_buffer(_impl, src, size, offset);
}
