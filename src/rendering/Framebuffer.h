#pragma once

#include "../container/Array.h"
#include "../macros.h"
#include "RenderPass.h"
#include "Texture.h"
#include "Vulkan.h"

namespace L {
  class Framebuffer {
    L_NOCOPY(Framebuffer)
  private:
    Array<Texture> _textures;
    Array<VkClearValue> _clear_values;
    uint32_t _width, _height;
    VkFramebuffer _framebuffer;
    const RenderPass& _render_pass;
  public:
    Framebuffer(uint32_t width, uint32_t height, const RenderPass& render_pass);
    ~Framebuffer();

    void resize(uint32_t width, uint32_t height);

    void begin(VkCommandBuffer);
    void end(VkCommandBuffer);

    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline VkImageView image_view(uintptr_t i) const { return _textures[i]; }
    inline const RenderPass& render_pass() const { return _render_pass; }
  };
}
