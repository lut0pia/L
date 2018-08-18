#include "RenderPass.h"

using namespace L;

RenderPass::RenderPass(const Array<VkFormat>& formats, bool present) : _formats(formats) {
  const size_t format_count(formats.size());
  _has_depth = false;
  for(uintptr_t i(0); i<format_count; i++) {
    VkAttachmentDescription attachment_desc {};
    attachment_desc.format = formats[i];
    attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkAttachmentReference attachment_ref {};
    attachment_ref.attachment = i;

    if(Vulkan::is_depth_format(formats[i])) {
      L_ASSERT(i==format_count-1);
      attachment_desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      _has_depth = true;
    } else {
      attachment_desc.finalLayout = present ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    _attachment_descriptions.push(attachment_desc);
    _attachment_references.push(attachment_ref);
  }

  VkSubpassDescription subpass_desc {};
  subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_desc.colorAttachmentCount = color_attachment_count();
  subpass_desc.pColorAttachments = _attachment_references.begin();
  if(_has_depth)
    subpass_desc.pDepthStencilAttachment = _attachment_references.end()-1;

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = _attachment_descriptions.size();
  render_pass_info.pAttachments = _attachment_descriptions.begin();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass_desc;

  L_VK_CHECKED(vkCreateRenderPass(Vulkan::device(), &render_pass_info, nullptr, &_render_pass));
}
RenderPass::~RenderPass() {
  vkDestroyRenderPass(Vulkan::device(), _render_pass, nullptr);
}

const RenderPass& RenderPass::geometry_pass() {
  static RenderPass render_pass({
    VK_FORMAT_B8G8R8A8_UNORM, // Color+Metal
    VK_FORMAT_R16G16B16A16_UNORM, // Normal+Roughness+Emission
    VK_FORMAT_D24_UNORM_S8_UINT, // Depth
  });
  return render_pass;
}
const RenderPass& RenderPass::light_pass() {
  static RenderPass render_pass(Array<VkFormat>{
    VK_FORMAT_R16G16B16A16_SFLOAT, // Light
  });
  return render_pass;
}
const RenderPass& RenderPass::present_pass() {
  static RenderPass render_pass(Array<VkFormat>{
    VK_FORMAT_B8G8R8A8_UNORM, // Final color
  }, true);
  return render_pass;
}
