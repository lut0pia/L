#include "VulkanRenderer.h"

#include <string.h>

#include "VulkanBuffer.h"

using namespace L;

static void transition_layout(VulkanTexture* tex, VkCommandBuffer cmd_buffer, VkImageLayout new_layout) {
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = tex->layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = tex->image;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = tex->mip_count;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = tex->layer_count;

  if(new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if(VulkanRenderer::get()->is_stencil_format(tex->format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags src_stage;
  VkPipelineStageFlags dst_stage;

  if(tex->layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if(tex->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if(tex->layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if(tex->layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    return error("Unsupported Texture layout transition");
  }

  vkCmdPipelineBarrier(cmd_buffer, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  tex->layout = new_layout;
}


TextureImpl* VulkanRenderer::create_texture(uint32_t width, uint32_t height, L::RenderFormat format, const void** mip_data, size_t* mip_size, size_t mip_count) {
  const bool depth_texture(Renderer::is_depth_format(format));
  const bool compressed_texture(Renderer::is_block_format(format));
  const bool cubemap(width*6==height);

  VulkanTexture* tex = Memory::new_type<VulkanTexture>();
  tex->width = width;
  tex->height = height;
  tex->mip_count = uint32_t(mip_count);
  tex->is_depth = depth_texture;

  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height = cubemap ? width : height;
  image_info.extent.depth = 1;
  image_info.mipLevels = tex->mip_count;
  image_info.arrayLayers = tex->layer_count = cubemap ? 6 : 1;
  image_info.format = tex->format = to_vk_format(format);
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if(depth_texture) image_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  if(!depth_texture && !compressed_texture) image_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  if(cubemap) image_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

  L_VK_CHECKED(vkCreateImage(VulkanRenderer::get()->device(), &image_info, nullptr, &tex->image));

  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(VulkanRenderer::get()->device(), tex->image, &requirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = requirements.size;
  allocInfo.memoryTypeIndex = VulkanRenderer::find_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  L_VK_CHECKED(vkAllocateMemory(_device, &allocInfo, nullptr, &tex->memory));

  vkBindImageMemory(_device, tex->image, tex->memory, 0);

  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = tex->image;
  viewInfo.viewType = cubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = tex->format;
  viewInfo.subresourceRange.aspectMask = depth_texture ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = tex->mip_count;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = tex->layer_count;

  L_VK_CHECKED(vkCreateImageView(_device, &viewInfo, nullptr, &tex->view));

  if(mip_data) { // Optional loading of texture data
    for(uint32_t mip_level = 0; mip_level < mip_count; mip_level++) {
      load_texture(tex, mip_data[mip_level], mip_size[mip_level], 0, Vector3i(width >> mip_level, height >> mip_level, 1), mip_level);
    }
  }

  return tex;
}
void VulkanRenderer::destroy_texture(TextureImpl* tex) {
  VulkanTexture* vk_tex = (VulkanTexture*)tex;
  VulkanRenderer::destroy_image(vk_tex->image, vk_tex->memory);
  Memory::delete_type(vk_tex);
}

void VulkanRenderer::load_texture(TextureImpl* tex, const void* data, size_t size, const L::Vector3i& offset, const L::Vector3i& extent, uint32_t mip_level) {
  VulkanTexture* vk_tex = (VulkanTexture*)tex;
  VulkanBuffer buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

  void* mapped_mem;
  vkMapMemory(_device, buffer, 0, size, 0, &mapped_mem);
  memcpy(mapped_mem, data, size);
  vkUnmapMemory(_device, buffer);

  VkCommandBuffer cmd(begin_command_buffer());
  transition_layout(vk_tex, cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = mip_level;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = vk_tex->layer_count;

  region.imageOffset = {int32_t(offset.x()), int32_t(offset.y()), int32_t(offset.z())};
  region.imageExtent = {uint32_t(extent.x()), uint32_t(extent.y()), uint32_t(extent.z())};

  vkCmdCopyBufferToImage(
    cmd,
    buffer,
    vk_tex->image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    1,
    &region
  );

  transition_layout(vk_tex, cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  end_command_buffer(cmd);
}
