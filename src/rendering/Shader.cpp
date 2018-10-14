#include "Shader.h"

#include "../dev/profiling.h"

using namespace L;

Shader::Shader(const Intermediate& intermediate) : _stage(intermediate.stage), _bindings(intermediate.bindings) {
  VkShaderModuleCreateInfo create_info {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = intermediate.binary.size();
  create_info.pCode = (uint32_t*)intermediate.binary.data();

  L_VK_CHECKED(vkCreateShaderModule(Vulkan::device(), &create_info, nullptr, &_module));
}
Shader::~Shader() {
  vkDestroyShaderModule(Vulkan::device(), _module, nullptr);
}
