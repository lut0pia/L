#include "Framebuffer.h"

#include "Texture.h"
#include "../stream/CFileStream.h"

using namespace L;

Framebuffer::Framebuffer(uint32_t width, uint32_t height, const RenderPass& render_pass)
  : _width(width), _height(height), _render_pass(render_pass) {
  Array<VkImageView> attachment_views;

  for(VkFormat format : render_pass.formats()) {
    _textures.push(width, height, format);
    attachment_views.push(_textures.back());

    if(Vulkan::is_depth_format(format)) {
      _clear_values.push(VkClearValue {1.f, 0});
    } else {
      _clear_values.push(VkClearValue {0.f, 0.f, 0.f, 0.f});
    }
  }

  { // Create framebuffer
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = _render_pass;
    create_info.attachmentCount = uint32_t(attachment_views.size());
    create_info.pAttachments = attachment_views.begin();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = 1;

    L_VK_CHECKED(vkCreateFramebuffer(Vulkan::device(), &create_info, nullptr, &_framebuffer));
  }
}
Framebuffer::~Framebuffer() {
  Vulkan::destroy_framebuffer(_framebuffer);
}

void Framebuffer::resize(uint32_t width, uint32_t height) {
  this->~Framebuffer();
  new(this)Framebuffer(width, height, _render_pass);
}

void Framebuffer::begin(VkCommandBuffer cmd_buffer) {
  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = _render_pass;
  render_pass_info.framebuffer = _framebuffer;
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = VkExtent2D {_width, _height};
  render_pass_info.clearValueCount = uint32_t(_clear_values.size());
  render_pass_info.pClearValues = _clear_values.begin();

  vkCmdBeginRenderPass(cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}
void Framebuffer::end(VkCommandBuffer cmd_buffer) {
  vkCmdEndRenderPass(cmd_buffer);
}
