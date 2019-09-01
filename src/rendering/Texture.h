#pragma once

#include "../container/Buffer.h"
#include "../macros.h"
#include "Vulkan.h"

namespace L {
  class Texture {
    L_NOCOPY(Texture)
  protected:
    uint32_t _width, _height, _layer_count;
    VkFormat _format;
    VkImageLayout _layout;
    VkDeviceMemory _memory;
    VkImage _image;
    VkImageView _view;
  public:
    struct Intermediate {
      Buffer binary;
      uint32_t width, height;
      VkFormat format;
    };
    inline Texture(const Intermediate& intermediate)
      : Texture(intermediate.width, intermediate.height, intermediate.format, intermediate.binary, intermediate.binary.size()) {
    }
    Texture(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, const void* data = nullptr, size_t size = 0);
    ~Texture();
    inline void load(const void* data, size_t size) { load(data, size, {}, {_width,_height,1}); }
    void load(const void* data, size_t size, VkOffset3D offset, VkExtent3D extent);
    void transition_layout(VkCommandBuffer cmd_buffer, VkImageLayout new_layout);

    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline VkFormat format() const { return _format; }
    inline operator VkImage() { return _image; }
    inline operator VkDeviceMemory() { return _memory; }
    inline operator VkImageView() const { return _view; }

    static const Texture& black();

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.binary <= v.width <= v.height <= v.format; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.binary >= v.width >= v.height >= v.format; }
  };
}
