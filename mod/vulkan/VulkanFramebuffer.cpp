#include "VulkanRenderer.h"

#include <L/src/rendering/RenderPass.h>

using namespace L;

FramebufferImpl* VulkanRenderer::create_framebuffer(uint32_t width, uint32_t height, const L::RenderPass& render_pass) {
  VulkanFramebuffer* framebuffer = Memory::new_type<VulkanFramebuffer>();
  framebuffer->render_pass = (VkRenderPass)render_pass.get_impl();
  framebuffer->width = width;
  framebuffer->height = height;

  Array<VkImageView> attachment_views;
  for(RenderFormat format : render_pass.formats()) {
    VulkanTexture* texture = (VulkanTexture*)create_texture(width, height, format, nullptr, 0);
    framebuffer->textures.push(texture);
    attachment_views.push(texture->view);

    if(is_depth_format(format)) {
      framebuffer->clear_values.push(VkClearValue {1.f, 0});
    } else {
      framebuffer->clear_values.push(VkClearValue {0.f, 0.f, 0.f, 0.f});
    }
  }

  { // Create framebuffer
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = framebuffer->render_pass;
    create_info.attachmentCount = uint32_t(attachment_views.size());
    create_info.pAttachments = attachment_views.begin();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = 1;

    L_VK_CHECKED(vkCreateFramebuffer(_device, &create_info, nullptr, &framebuffer->framebuffer));
  }

  return framebuffer;
}
void VulkanRenderer::destroy_framebuffer(L::FramebufferImpl* framebuffer) {
  VulkanFramebuffer* vk_framebuffer = (VulkanFramebuffer*)framebuffer;
  destroy_framebuffer(vk_framebuffer->framebuffer);
  Memory::delete_type(vk_framebuffer);
}
void VulkanRenderer::begin_framebuffer(L::FramebufferImpl* framebuffer, L::RenderCommandBuffer* cmd_buffer) {
  VulkanFramebuffer* vk_framebuffer = (VulkanFramebuffer*)framebuffer;

  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = vk_framebuffer->render_pass;
  render_pass_info.framebuffer = vk_framebuffer->framebuffer;
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = VkExtent2D {vk_framebuffer->width, vk_framebuffer->height};
  render_pass_info.clearValueCount = uint32_t(vk_framebuffer->clear_values.size());
  render_pass_info.pClearValues = vk_framebuffer->clear_values.begin();

  vkCmdBeginRenderPass((VkCommandBuffer)cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}
void VulkanRenderer::end_framebuffer(L::FramebufferImpl*, L::RenderCommandBuffer* cmd_buffer) {
  vkCmdEndRenderPass((VkCommandBuffer)cmd_buffer);
}
