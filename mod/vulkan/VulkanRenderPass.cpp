#include "VulkanRenderer.h"

using namespace L;

RenderPassImpl* VulkanRenderer::create_render_pass(const RenderFormat* formats, size_t format_count, bool present) {
  Array<VkAttachmentReference> attachment_references;
  Array<VkAttachmentDescription> attachment_descriptions;
  bool has_depth = false;

  for(uintptr_t i = 0; i < format_count; i++) {
    VkAttachmentDescription attachment_desc {};
    attachment_desc.format = to_vk_format(formats[i]);
    attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkAttachmentReference attachment_ref {};
    attachment_ref.attachment = uint32_t(i);

    if(Renderer::is_depth_format(formats[i])) {
      L_ASSERT(i==format_count-1);
      attachment_desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      has_depth = true;
    } else {
      attachment_desc.finalLayout = present ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    attachment_descriptions.push(attachment_desc);
    attachment_references.push(attachment_ref);
  }

  VkSubpassDescription subpass_desc {};
  subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_desc.colorAttachmentCount = uint32_t(format_count) - has_depth;
  subpass_desc.pColorAttachments = attachment_references.begin();
  if(has_depth) {
    subpass_desc.pDepthStencilAttachment = attachment_references.end() - 1;
  }

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = uint32_t(attachment_descriptions.size());
  render_pass_info.pAttachments = attachment_descriptions.begin();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass_desc;

  VkRenderPass render_pass;
  L_VK_CHECKED(vkCreateRenderPass(_device, &render_pass_info, nullptr, &render_pass));

  return (RenderPassImpl*)render_pass;
}
void VulkanRenderer::destroy_render_pass(RenderPassImpl* render_pass) {
  vkDestroyRenderPass(_device, (VkRenderPass)render_pass, nullptr);
}
