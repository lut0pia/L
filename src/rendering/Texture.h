#pragma once

#include "../container/Buffer.h"
#include "../macros.h"
#include "Vulkan.h"

namespace L {
  class Texture {
    L_NOCOPY(Texture)
  protected:
    uint32_t _width, _height;
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
    inline Texture(const Intermediate& intermediate) : Texture(intermediate.width, intermediate.height, intermediate.format) {
      load(intermediate.binary);
    }
    Texture(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
    ~Texture();
    inline void load(const void* pixels) { load(pixels, {}, {_width,_height,1}); }
    void load(const void* pixels, VkOffset3D offset, VkExtent3D extent);
    void transition_layout(VkCommandBuffer cmd_buffer, VkImageLayout new_layout);

    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline VkFormat format() const { return _format; }
    inline operator VkImage() { return _image; }
    inline operator VkDeviceMemory() { return _memory; }
    inline operator const VkImageView() const { return _view; }

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.binary <= v.width <= v.height <= v.format; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.binary >= v.width >= v.height >= v.format; }
  };
}
