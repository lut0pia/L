#include "VulkanBuffer.h"

using namespace L;

L::UniformBufferImpl* VulkanRenderer::create_uniform_buffer(size_t size) {
  return (UniformBufferImpl*)Memory::new_type<VulkanBuffer>(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}
void VulkanRenderer::destroy_uniform_buffer(UniformBufferImpl* uniform_buffer) {
  Memory::delete_type((VulkanBuffer*)uniform_buffer);
}
void VulkanRenderer::load_uniform_buffer(UniformBufferImpl* uniform_buffer, const void* src, size_t size, size_t offset) {
  VulkanBuffer* vk_uniform_buffer = (VulkanBuffer*)uniform_buffer;
  vk_uniform_buffer->load(src, size, offset);
}
