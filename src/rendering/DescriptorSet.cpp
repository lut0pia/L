#include "DescriptorSet.h"

#include "../engine/Engine.h"

using namespace L;

static Array<VkDescriptorSet> garbage_sets;

DescriptorSet::DescriptorSet(const Pipeline& pipeline) : _pipeline(pipeline) {
  VkDescriptorSetLayout layouts[] = {pipeline.desc_set_layout()};
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = Vulkan::descriptor_pool();
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;

  L_VK_CHECKED(vkAllocateDescriptorSets(Vulkan::device(), &allocInfo, &_set));

  set_descriptor("Shared", VkDescriptorBufferInfo {Engine::shared_uniform(), 0, Engine::shared_uniform().size()});

  for(const auto& binding : pipeline.bindings()) {
    static const Symbol shared_symbol("Shared");
    if(binding.binding>0 && binding.type==Shader::BindingType::UniformBlock && binding.name!=shared_symbol) {
      _buffers.push(binding.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
      set_descriptor(binding.name, VkDescriptorBufferInfo {_buffers.back(), 0, _buffers.back().size()});
    }
  }
}
DescriptorSet::~DescriptorSet() {
  garbage_sets.push(_set);
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

void DescriptorSet::advance_frame() {
  vkFreeDescriptorSets(Vulkan::device(), Vulkan::descriptor_pool(), garbage_sets.size(), garbage_sets.begin());
  garbage_sets.clear();
}
