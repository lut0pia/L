#include "DescriptorSet.h"

#include "../dev/profiling.h"

using namespace L;

DescriptorSet::DescriptorSet(const Pipeline& pipeline) : _set(0), _pipeline(pipeline) {
  L_SCOPE_MARKER("DescriptorSet creation");

  if(!Vulkan::find_desc_set(pipeline, _set)) {
    VkDescriptorSetLayout layouts[] = {pipeline.desc_set_layout()};
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = Vulkan::descriptor_pool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    L_VK_CHECKED(vkAllocateDescriptorSets(Vulkan::device(), &allocInfo, &_set));
  }

  for(const auto& binding : pipeline.bindings()) {
    static const Symbol shared_symbol("Shared");
    if(binding.binding>0 && binding.type==Shader::BindingType::Uniform && binding.name!=shared_symbol) {
      _buffers.push(binding.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
      set_descriptor(binding.name, _buffers.back().descriptor_info());
    }
  }
}
DescriptorSet::~DescriptorSet() {
  Vulkan::destroy_desc_set(_pipeline, _set);
}

bool DescriptorSet::set_descriptor(const Symbol& name, VkDescriptorBufferInfo buffer_info) {
  if(const Shader::Binding* binding = _pipeline.find_binding(name)) {
    VkWriteDescriptorSet desc_write {};
    desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    desc_write.dstSet = _set;
    desc_write.dstBinding = binding->binding;
    desc_write.dstArrayElement = 0;
    desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    desc_write.descriptorCount = 1;
    desc_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(Vulkan::device(), 1, &desc_write, 0, nullptr);

    return true;
  } else return false;
}
bool DescriptorSet::set_descriptor(const Symbol& name, VkDescriptorImageInfo image_info) {
  if(const Shader::Binding* binding = _pipeline.find_binding(name)) {
    VkWriteDescriptorSet desc_write {};
    desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    desc_write.dstSet = _set;
    desc_write.dstBinding = binding->binding;
    desc_write.dstArrayElement = 0;
    desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    desc_write.descriptorCount = 1;
    desc_write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(Vulkan::device(), 1, &desc_write, 0, nullptr);

    return true;
  } else return false;
}
bool DescriptorSet::set_value(const Symbol& name, const void* data, size_t size) {
  if(const Shader::Binding* binding = _pipeline.find_binding(name)) {
    _buffers[0].load(data, size, binding->offset);
    return true;
  } else return false;
}
