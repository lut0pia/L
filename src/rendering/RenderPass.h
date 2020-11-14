#pragma once

#include "../container/Array.h"
#include "../macros.h"
#include "Renderer.h"

namespace L {
  class RenderPass {
    L_NOCOPY(RenderPass)
  private:
    Array<RenderFormat> _formats;
    RenderPassImpl* _impl;
    bool _has_depth;
  public:
    RenderPass(const Array<RenderFormat>& formats, bool present = false);
    ~RenderPass();

    inline RenderPassImpl* get_impl() const { return _impl; }
    inline uint32_t color_attachment_count() const { return uint32_t(_formats.size())-_has_depth; }
    inline const Array<RenderFormat> formats() const { return _formats; }

    static const RenderPass& geometry_pass();
    static const RenderPass& light_pass();
    static const RenderPass& present_pass();
  };
}
