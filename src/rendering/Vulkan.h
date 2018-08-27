#pragma once

#include <vulkan/vulkan.h>

#define L_VK_EXT_FUNC(name,...) (PFN_##name(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT")))(__VA_ARGS__)
#define L_VK_CHECKED(...) {VkResult result(__VA_ARGS__);L_ASSERT_MSG(result==VK_SUCCESS,Vulkan::result_str(result));}

namespace L {
  namespace Vulkan {
    void init();

    const char* result_str(VkResult result);
    VkFormat find_supported_format(VkFormat* candidates, size_t candidate_count, VkFormatFeatureFlags features);
    bool is_depth_format(VkFormat format);
    bool is_stencil_format(VkFormat format);
    bool is_block_format(VkFormat format);
    uint32_t format_size(VkFormat format);
    uint32_t find_memory_type(uint32_t type_bits, VkMemoryPropertyFlags property_flags);

    VkCommandBuffer begin_render_command_buffer();
    void end_render_command_buffer();
    void begin_present_pass();
    void end_present_pass();
    VkCommandBuffer begin_command_buffer();
    void end_command_buffer(VkCommandBuffer);

    bool find_buffer(VkDeviceSize, VkBufferUsageFlagBits, VkBuffer&, VkDeviceMemory&);
    void destroy_buffer(VkDeviceSize, VkBufferUsageFlagBits, VkBuffer, VkDeviceMemory);
    bool find_desc_set(VkPipeline, VkDescriptorSet&);
    void destroy_desc_set(VkPipeline, VkDescriptorSet);
    void destroy_framebuffer(VkFramebuffer);
    void destroy_image(VkImage, VkDeviceMemory);

    VkDevice device();
    VkDescriptorPool descriptor_pool();
    VkSampler sampler();
  }
}
