#pragma once

#include "../container/Array.h"
#include "../macros.h"
#include "Renderer.h"
#include "RenderPass.h"

namespace L {
  class Framebuffer {
    L_NOCOPY(Framebuffer)
  private:
    FramebufferImpl* _impl;
    uint32_t _width, _height;
    const RenderPass& _render_pass;
  public:
    Framebuffer(uint32_t width, uint32_t height, const RenderPass& render_pass);
    ~Framebuffer();

    void resize(uint32_t width, uint32_t height);

    void begin(RenderCommandBuffer*);
    void end(RenderCommandBuffer*);

    inline FramebufferImpl* get_impl() const { return _impl; }
    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline const RenderPass& render_pass() const { return _render_pass; }
  };
}
