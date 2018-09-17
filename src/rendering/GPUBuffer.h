#pragma once

#include "Vulkan.h"
#include "../macros.h"

namespace L {
  class GPUBuffer {
    L_NOCOPY(GPUBuffer)
  protected:
    VkBuffer _buffer;
    VkDeviceMemory _memory;
    VkDeviceSize _size;
    VkBufferUsageFlagBits _usage;
  public:
    GPUBuffer(size_t size, VkBufferUsageFlagBits usage);
    ~GPUBuffer();

    inline void load(const void* data) { load(data, _size); }
    void load(const void* data, size_t size, size_t offset = 0);
    template <class T> void load_item(const T& datum, size_t offset = 0) { load(&datum, sizeof(T), offset); }

    inline VkDeviceSize size() const { return _size; }
    inline operator VkBuffer() { return _buffer; }
    inline operator VkDeviceMemory() { return _memory; }
    inline VkDescriptorBufferInfo descriptor_info() const { return {_buffer,0,_size}; }
  };
}
