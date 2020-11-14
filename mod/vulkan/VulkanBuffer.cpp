#include "VulkanBuffer.h"

#include <string.h>

#include <L/src/dev/debug.h>

using namespace L;

VulkanBuffer::VulkanBuffer(size_t size, VkBufferUsageFlagBits usage) : _size(size), _usage(usage) {
  if(VulkanRenderer::get()->find_buffer(_size, _usage, _buffer, _memory))
    return;
  
  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = _size;
  create_info.usage = _usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  L_VK_CHECKED(vkCreateBuffer(VulkanRenderer::get()->device(), &create_info, nullptr, &_buffer));

  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(VulkanRenderer::get()->device(), _buffer, &requirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = requirements.size;
  allocInfo.memoryTypeIndex = VulkanRenderer::get()->find_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  L_VK_CHECKED(vkAllocateMemory(VulkanRenderer::get()->device(), &allocInfo, nullptr, &_memory));
  L_VK_CHECKED(vkBindBufferMemory(VulkanRenderer::get()->device(), _buffer, _memory, 0));
}
VulkanBuffer::~VulkanBuffer() {
  VulkanRenderer::get()->destroy_buffer(_size, _usage, _buffer, _memory);
}

void VulkanBuffer::load(const void* src, size_t size, size_t offset) {
  void* dst;
  vkMapMemory(VulkanRenderer::get()->device(), _memory, offset, size, 0, &dst);
  memcpy(dst, src, size);
  vkUnmapMemory(VulkanRenderer::get()->device(), _memory);
}
