#pragma once

#include "../container/Array.h"
#include "../macros.h"
#include "Vulkan.h"

namespace L {
  class RenderPass {
    L_NOCOPY(RenderPass)
  private:
    Array<VkFormat> _formats;
    Array<VkAttachmentReference> _attachment_references;
    Array<VkAttachmentDescription> _attachment_descriptions;
    VkRenderPass _render_pass;
    bool _has_depth;
  public:
    RenderPass(const Array<VkFormat>& formats, bool present = false);
    ~RenderPass();

    inline uint32_t color_attachment_count() const { return uint32_t(_attachment_descriptions.size())-_has_depth; }
    inline const Array<VkFormat> formats() const { return _formats; }
    operator VkRenderPass() const { return _render_pass; }

    static const RenderPass& geometry_pass();
    static const RenderPass& light_pass();
    static const RenderPass& present_pass();
  };
}
