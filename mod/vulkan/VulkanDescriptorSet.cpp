#include "VulkanRenderer.h"

#include "VulkanBuffer.h"

using namespace L;

DescriptorSetImpl* VulkanRenderer::create_descriptor_set(PipelineImpl* pipeline) {
  VulkanPipeline* vk_pipeline = (VulkanPipeline*)pipeline;
  VkDescriptorSet desc_set;

  // Try to find already allocated descriptor set
  for(uintptr_t i = 0; i < free_sets.size(); i++) {
    const FlyingSet& flying_set = free_sets[i];
    if(flying_set.pipeline == vk_pipeline->pipeline) {
      desc_set = flying_set.set;
      free_sets.erase_fast(i);
      return (DescriptorSetImpl*)desc_set;
    }
  }

  // Allocate new one
  VkDescriptorSetLayout layouts[] = {vk_pipeline->desc_set_layout};
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = _descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = layouts;

  L_VK_CHECKED(vkAllocateDescriptorSets(_device, &alloc_info, &desc_set));
  return (DescriptorSetImpl*)desc_set;
}
void VulkanRenderer::destroy_descriptor_set(DescriptorSetImpl* desc_set, PipelineImpl* pipeline) {
  VulkanPipeline* vk_pipeline = (VulkanPipeline*)pipeline;
  used_sets.push(FlyingSet {(VkDescriptorSet)desc_set, vk_pipeline->pipeline});
}
void VulkanRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, TextureImpl* texture) {
  VulkanTexture* vk_texture = (VulkanTexture*)texture;
  VkDescriptorImageInfo image_info {_sampler, vk_texture->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
  if(vk_texture->is_depth) {
    image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  }
  VkWriteDescriptorSet desc_write {};
  desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  desc_write.dstSet = (VkDescriptorSet)desc_set;
  desc_write.dstBinding = binding.binding;
  desc_write.dstArrayElement = 0;
  desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  desc_write.descriptorCount = 1;
  desc_write.pImageInfo = &image_info;

  vkUpdateDescriptorSets(_device, 1, &desc_write, 0, nullptr);
}
void VulkanRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, UniformBufferImpl* uniform_buffer) {
  VulkanBuffer* vk_uniform_buffer = (VulkanBuffer*)uniform_buffer;
  VkDescriptorBufferInfo buffer_info {*vk_uniform_buffer, 0, vk_uniform_buffer->size()};
  VkWriteDescriptorSet desc_write {};
  desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  desc_write.dstSet = (VkDescriptorSet)desc_set;
  desc_write.dstBinding = binding.binding;
  desc_write.dstArrayElement = 0;
  desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  desc_write.descriptorCount = 1;
  desc_write.pBufferInfo = &buffer_info;

  vkUpdateDescriptorSets(_device, 1, &desc_write, 0, nullptr);
}
void VulkanRenderer::update_descriptor_set(DescriptorSetImpl* desc_set, const ShaderBinding& binding, FramebufferImpl* framebuffer, int32_t texture_index) {
  VulkanFramebuffer* vk_framebuffer = (VulkanFramebuffer*)framebuffer;
  update_descriptor_set(desc_set, binding, vk_framebuffer->textures[texture_index]);
}
