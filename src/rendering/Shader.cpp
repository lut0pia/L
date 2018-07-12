#include "Shader.h"

using namespace L;

Shader::Shader(const void* binary, size_t size, VkShaderStageFlagBits stage) : _stage(stage) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = size;
  create_info.pCode = reinterpret_cast<const uint32_t*>(binary);

  L_VK_CHECKED(vkCreateShaderModule(Vulkan::device(), &create_info, nullptr, &_module));
}
Shader::~Shader() {
  vkDestroyShaderModule(Vulkan::device(), _module, nullptr);
}
