#include "VulkanRenderer.h"

using namespace L;

ShaderImpl* VulkanRenderer::create_shader(L::ShaderStage stage, const void* binary, size_t binary_size) {
  VulkanShader* shader = Memory::new_type<VulkanShader>();
  shader->stage = to_vk_shader_stage(stage);

  VkShaderModuleCreateInfo create_info {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = binary_size;
  create_info.pCode = (uint32_t*)binary;

  L_VK_CHECKED(vkCreateShaderModule(_device, &create_info, nullptr, &shader->module));

  return shader;
}
void VulkanRenderer::destroy_shader(ShaderImpl* shader) {
  VulkanShader* vk_shader = (VulkanShader*)shader;
  vkDestroyShaderModule(_device, vk_shader->module, nullptr);
}
