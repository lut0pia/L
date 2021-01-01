#include "VulkanRenderer.h"

using namespace L;

RenderPassImpl* VulkanRenderer::create_render_pass(const RenderFormat* formats, size_t format_count, bool present, bool depth_write) {
  VulkanRenderPass* render_pass = Memory::new_type<VulkanRenderPass>();
  Array<VkAttachmentReference> attachment_references;
  Array<VkAttachmentDescription> attachment_descriptions;
  bool has_depth = false;

  for(uintptr_t i = 0; i < format_count; i++) {
    const bool is_depth = is_depth_format(formats[i]);
    const bool should_clear = !is_depth || depth_write;
    VkAttachmentDescription attachment_desc {};
    attachment_desc.format = to_vk_format(formats[i]);
    attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_desc.loadOp = should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment_desc.storeOp = should_clear ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_desc.initialLayout = should_clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference attachment_ref {};
    attachment_ref.attachment = uint32_t(i);

    if(is_depth) {
      L_ASSERT(i == format_count - 1);
      attachment_desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = depth_write ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      has_depth = true;
    } else {
      attachment_desc.finalLayout = present ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      render_pass->color_attachment_count++;
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

  L_VK_CHECKED(vkCreateRenderPass(_device, &render_pass_info, nullptr, &render_pass->render_pass));

  return render_pass;
}
void VulkanRenderer::destroy_render_pass(RenderPassImpl* render_pass) {
  VulkanRenderPass* vk_render_pass = (VulkanRenderPass*)render_pass;
  vkDestroyRenderPass(_device, vk_render_pass->render_pass, nullptr);
  Memory::delete_type(vk_render_pass);
}
