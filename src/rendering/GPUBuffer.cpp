#include "GPUBuffer.h"

#include <string.h>
#include "../dev/debug.h"

using namespace L;

GPUBuffer::GPUBuffer(size_t size, VkBufferUsageFlagBits usage) : _size(size), _usage(usage) {
  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = _size;
  create_info.usage = _usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  L_VK_CHECKED(vkCreateBuffer(Vulkan::device(), &create_info, nullptr, &_buffer));

  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(Vulkan::device(), _buffer, &requirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = requirements.size;
  allocInfo.memoryTypeIndex = Vulkan::find_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  L_VK_CHECKED(vkAllocateMemory(Vulkan::device(), &allocInfo, nullptr, &_memory));
  L_VK_CHECKED(vkBindBufferMemory(Vulkan::device(), _buffer, _memory, 0));
}
GPUBuffer::~GPUBuffer() {
  vkDestroyBuffer(Vulkan::device(), _buffer, nullptr);
  vkFreeMemory(Vulkan::device(), _memory, nullptr);
}

void GPUBuffer::load(const void* src, size_t size, size_t offset) {
  void* dst;
  vkMapMemory(Vulkan::device(), _memory, offset, size, 0, &dst);
  memcpy(dst, src, size);
  vkUnmapMemory(Vulkan::device(), _memory);
}
