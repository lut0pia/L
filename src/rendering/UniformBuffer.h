#pragma once

#include "Renderer.h"
#include "../macros.h"

namespace L {
  class UniformBuffer {
    L_NOCOPY(UniformBuffer)
  protected:
    UniformBufferImpl* _impl;
    size_t _size;
  public:
    UniformBuffer(size_t size);
    ~UniformBuffer();

    inline void load(const void* data) { load(data, _size); }
    void load(const void* data, size_t size, size_t offset = 0);
    template <class T> void load_item(const T& datum, size_t offset = 0) { load(&datum, sizeof(T), offset); }

    inline UniformBufferImpl* get_impl() const { return _impl; }
    inline size_t size() const { return _size; }
  };
}
