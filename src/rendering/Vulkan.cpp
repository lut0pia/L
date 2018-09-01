#include "Vulkan.h"

#include "../dev/debug.h"
#include "../macros.h"
#include "../stream/CFileStream.h"
#include "DescriptorSet.h"
#include "../parallelism/Lock.h"

#include "../dev/profiling.h"

using namespace L;

namespace L {
  namespace Vulkan {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    VkDevice _device;
    VkQueue queue;
    VkSurfaceKHR surface;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceFormatKHR surface_format;
    VkSwapchainKHR swapchain;
    VkViewport _viewport = {};
    VkImage swapchain_images[2];
    VkImageView swapchain_image_views[2];
    VkFramebuffer framebuffers[2];
    VkCommandPool* command_pool;
    VkFence* command_fence;
    VkCommandBuffer render_command_buffers[2];
    VkDescriptorPool _descriptor_pool;
    VkSampler _sampler;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    Lock queue_lock;

    struct FlyingBuffer {
      VkBuffer buffer;
      VkDeviceMemory memory;
      VkDeviceSize size;
      VkBufferUsageFlagBits usage;
    };
    struct FlyingSet {
      VkDescriptorSet set;
      VkPipeline pipeline;
    };
    struct GarbageImage {
      VkImage image;
      VkDeviceMemory memory;
    };
    static Array<FlyingBuffer> used_buffers, free_buffers;
    static Array<FlyingSet> used_sets, free_sets;
    static Array<VkFramebuffer> garbage_framebuffers;
    static Array<GarbageImage> garbage_images;

#ifdef L_DEBUG
    VkDebugReportCallbackEXT debug_report_callback;
#endif

    // System-specific function
    void create_surface(VkSurfaceKHR*);
    const char* extra_extension();
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
  err << "Vulkan debug: " << msg << '\n';
  return VK_FALSE;
}

void Vulkan::init() {
  putenv("DISABLE_VK_LAYER_VALVE_steam_overlay_1=1");
  L_SCOPE_MARKER("Vulkan::init");
  const char* validation_layers[] = {
    "VK_LAYER_LUNARG_standard_validation",
    //"VK_LAYER_RENDERDOC_Capture",
  };

  { // Create instance
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pEngineName = "L Engine";
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    const char* required_extensions[] = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      extra_extension(),
#ifdef L_DEBUG
      VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    };
    create_info.enabledExtensionCount = L_COUNT_OF(required_extensions);
    create_info.ppEnabledExtensionNames = required_extensions;

#ifdef L_DEBUG
    create_info.enabledLayerCount = L_COUNT_OF(validation_layers);
    create_info.ppEnabledLayerNames = validation_layers;
#endif

    L_VK_CHECKED(vkCreateInstance(&create_info, nullptr, &instance));
  }

#ifdef L_DEBUG
  { // Create debug report callback
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debug_callback;

    L_VK_CHECKED(L_VK_EXT_FUNC(vkCreateDebugReportCallbackEXT, instance, &createInfo, nullptr, &debug_report_callback));
  }
#endif

  { // Select physical device
    VkPhysicalDevice physical_devices[16];
    uint32_t count(0);
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    L_ASSERT(count<L_COUNT_OF(physical_devices));
    vkEnumeratePhysicalDevices(instance, &count, physical_devices);
    L_ASSERT(count>0);

    uintptr_t best_physical_device(0);
    size_t best_physical_device_memory(0);

    { // Choose best physical device based on dedicated memory
      for(uintptr_t i(0); i<count; i++) {
        size_t physical_device_memory(0);
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &physical_device_memory_properties);
        for(uintptr_t j(0); j<physical_device_memory_properties.memoryHeapCount; j++) {
          if(physical_device_memory_properties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            physical_device_memory += physical_device_memory_properties.memoryHeaps[j].size;
          }
        }
        if(physical_device_memory>best_physical_device_memory) {
          best_physical_device_memory = physical_device_memory;
          best_physical_device = i;
        }
      }
      physical_device = physical_devices[best_physical_device];
    }

    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
    out << "GPU: " << physical_device_properties.deviceName << '\n'
      << "GPU memory: " << (best_physical_device_memory/(1<<20)) << "MB\n";
  }

  { // Create surface
    create_surface(&surface);
  }

  uint32_t queue_family_index(-1);
  { // Fetch queue family index
    uint32_t count(0);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
    VkQueueFamilyProperties queue_family_properties[16];
    L_ASSERT(count<L_COUNT_OF(queue_family_properties));
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, queue_family_properties);
    for(uint32_t i(0); i<count; i++) {
      VkBool32 present_support;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
      if(present_support && queue_family_properties[i].queueCount>0 && queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        queue_family_index = i;
    }
    L_ASSERT(queue_family_index!=-1);
  }

  { // Create device and fetch queue
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family_index;
    queue_create_info.queueCount = 1;
    float priority(1.f);
    queue_create_info.pQueuePriorities = &priority;

    const char* required_extensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkPhysicalDeviceFeatures enabled_features {};
    enabled_features.samplerAnisotropy = true;
    enabled_features.textureCompressionBC = true;

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.enabledExtensionCount = L_COUNT_OF(required_extensions);
    create_info.ppEnabledExtensionNames = required_extensions;
    create_info.pEnabledFeatures = &enabled_features;

    L_VK_CHECKED(vkCreateDevice(physical_device, &create_info, nullptr, &_device));

    vkGetDeviceQueue(_device, queue_family_index, 0, &queue);
  }

  { // Select surface format
    VkSurfaceFormatKHR surface_formats[16];
    uint32_t surface_format_count(0);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr);
    L_ASSERT(surface_format_count<L_COUNT_OF(surface_formats));
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats);
    for(uint32_t i(0); i<surface_format_count; i++)
      if(surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        surface_format = surface_formats[i];
  }

  { // Fetch surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    L_ASSERT(surface_capabilities.currentExtent.width!=-1);
  }

  { // Setup _viewport
    _viewport.x = 0.0f;
    _viewport.y = 0.0f;
    _viewport.width = (float)surface_capabilities.currentExtent.width;
    _viewport.height = (float)surface_capabilities.currentExtent.height;
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;
  }

  { // Create command pools
    const uint32_t command_pool_count(TaskSystem::fiber_count());
    command_pool = Memory::alloc_type<VkCommandPool>(command_pool_count);

    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue_family_index;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    for(uint32_t i(0); i<command_pool_count; i++) {
      L_VK_CHECKED(vkCreateCommandPool(_device, &create_info, nullptr, command_pool+i));
    }
  }

  { // Create command fences
    const uint32_t command_fence_count(TaskSystem::fiber_count());
    command_fence = Memory::alloc_type<VkFence>(command_fence_count);

    VkFenceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    for(uint32_t i(0); i<command_fence_count; i++) {
      L_VK_CHECKED(vkCreateFence(_device, &create_info, nullptr, command_fence+i));
    }
  }

  { // Create swapchain
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = 2;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = surface_capabilities.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform = surface_capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    L_VK_CHECKED(vkCreateSwapchainKHR(_device, &create_info, nullptr, &swapchain));

    { // Fetch swapchain images
      uint32_t swapchain_image_count;
      vkGetSwapchainImagesKHR(_device, swapchain, &swapchain_image_count, nullptr);
      L_ASSERT(swapchain_image_count==2);
      vkGetSwapchainImagesKHR(_device, swapchain, &swapchain_image_count, swapchain_images);
    }

    { // Create swapchain image views
      for(uint32_t i(0); i<2; i++) {
        VkImageViewCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swapchain_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = surface_format.format;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        L_VK_CHECKED(vkCreateImageView(_device, &create_info, nullptr, swapchain_image_views+i));
      }
    }
  }

  { // Create framebuffers
    for(size_t i = 0; i < L_COUNT_OF(swapchain_image_views); i++) {
      VkImageView attachments[] = {
        swapchain_image_views[i],
      };

      VkFramebufferCreateInfo framebufferInfo = {};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = RenderPass::present_pass();
      framebufferInfo.attachmentCount = L_COUNT_OF(attachments);
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = surface_capabilities.currentExtent.width;
      framebufferInfo.height = surface_capabilities.currentExtent.height;
      framebufferInfo.layers = 1;

      L_VK_CHECKED(vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &framebuffers[i]));
    }
  }

  { // Create command buffers
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = *command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)L_COUNT_OF(render_command_buffers);

    L_VK_CHECKED(vkAllocateCommandBuffers(_device, &allocInfo, render_command_buffers));
  }

  { // Create semaphores
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    L_VK_CHECKED(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &imageAvailableSemaphore));
    L_VK_CHECKED(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &renderFinishedSemaphore));
  }

  { // Create descriptor pool
    VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1<<12},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1<<10},
    };

    VkDescriptorPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = L_COUNT_OF(pool_sizes);
    create_info.pPoolSizes = pool_sizes;
    create_info.maxSets = 1<<16;
    create_info.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    L_VK_CHECKED(vkCreateDescriptorPool(_device, &create_info, nullptr, &_descriptor_pool));
  }

  { // Create default sampler
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    L_VK_CHECKED(vkCreateSampler(Vulkan::device(), &samplerInfo, nullptr, &_sampler));
  }
}

const char* Vulkan::result_str(VkResult result) {
#define CASE(v) case v: return #v
  switch(result) {
    CASE(VK_SUCCESS);
    CASE(VK_NOT_READY);
    CASE(VK_TIMEOUT);
    CASE(VK_EVENT_SET);
    CASE(VK_EVENT_RESET);
    CASE(VK_INCOMPLETE);
    CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
    CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
    CASE(VK_ERROR_INITIALIZATION_FAILED);
    CASE(VK_ERROR_DEVICE_LOST);
    CASE(VK_ERROR_MEMORY_MAP_FAILED);
    CASE(VK_ERROR_LAYER_NOT_PRESENT);
    CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
    CASE(VK_ERROR_FEATURE_NOT_PRESENT);
    CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
    CASE(VK_ERROR_TOO_MANY_OBJECTS);
    CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
    CASE(VK_ERROR_FRAGMENTED_POOL);
    CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
    CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
    CASE(VK_ERROR_SURFACE_LOST_KHR);
    CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
    CASE(VK_SUBOPTIMAL_KHR);
    CASE(VK_ERROR_OUT_OF_DATE_KHR);
    CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
    CASE(VK_ERROR_VALIDATION_FAILED_EXT);
    CASE(VK_ERROR_INVALID_SHADER_NV);
    CASE(VK_ERROR_FRAGMENTATION_EXT);
    CASE(VK_ERROR_NOT_PERMITTED_EXT);
    default: return "unknown";
  }
#undef CASE
}
VkFormat Vulkan::find_supported_format(VkFormat* candidates, size_t candidate_count, VkFormatFeatureFlags features) {
  for(uintptr_t i(0); i<candidate_count; i++) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(physical_device, candidates[i], &properties);
    if((properties.optimalTilingFeatures&features)==features)
      return candidates[i];
  }
  return VK_FORMAT_UNDEFINED;
}
bool Vulkan::is_depth_format(VkFormat format) {
  switch(format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return true;
    default: return false;
  }
}
bool Vulkan::is_stencil_format(VkFormat format) {
  switch(format) {
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return true;
    default: return false;
  }
}
bool Vulkan::is_block_format(VkFormat format) {
  return format >= VK_FORMAT_BC1_RGB_UNORM_BLOCK && format <= VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
}
uint32_t Vulkan::format_size(VkFormat format) {
  switch(format) {
    case VK_FORMAT_R8_UNORM:
      return 1;
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SFLOAT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
      return 4;
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R32G32_SFLOAT:
      return 8;
    case VK_FORMAT_R32G32B32_SFLOAT:
      return 12;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
      return 16;
    default: return 0;
  }
}
uint32_t Vulkan::find_memory_type(uint32_t type_bits, VkMemoryPropertyFlags property_flags) {
  for(uint32_t i(0); i<physical_device_memory_properties.memoryTypeCount; i++)
    if((type_bits&(1<<i)) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
      return i;
  error("Vulkan: failed to find suitable memory");
}
static uint32_t image_index;
VkCommandBuffer Vulkan::begin_render_command_buffer() {
  vkAcquireNextImageKHR(_device, swapchain, -1, imageAvailableSemaphore, VK_NULL_HANDLE, &image_index);
  VkCommandBuffer cmd_buffer(render_command_buffers[image_index]);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  beginInfo.pInheritanceInfo = nullptr; // Optional

  L_VK_CHECKED(vkBeginCommandBuffer(cmd_buffer, &beginInfo));
  return cmd_buffer;
}
void Vulkan::end_render_command_buffer() {
  VkCommandBuffer cmd_buffer(render_command_buffers[image_index]);

  L_VK_CHECKED(vkEndCommandBuffer(cmd_buffer));

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd_buffer;

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &image_index;

  {
    L_SCOPE_MARKER("Present");
    L_SCOPED_LOCK(queue_lock);
    L_VK_CHECKED(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    L_VK_CHECKED(vkQueuePresentKHR(queue, &presentInfo));
    vkQueueWaitIdle(queue);
  }

  for(const FlyingBuffer& buffer : used_buffers) {
    free_buffers.push(buffer);
  }
  used_buffers.clear();
  for(const FlyingSet& set : used_sets) {
    free_sets.push(set);
  }
  used_sets.clear();
  for(const VkFramebuffer& framebuffer : garbage_framebuffers) {
    vkDestroyFramebuffer(Vulkan::device(), framebuffer, nullptr);
  }
  garbage_framebuffers.clear();
  for(const GarbageImage& image : garbage_images) {
    vkDestroyImage(Vulkan::device(), image.image, nullptr);
    vkFreeMemory(Vulkan::device(), image.memory, nullptr);
  }
  garbage_images.clear();
}
void Vulkan::begin_present_pass() {
  VkCommandBuffer cmd_buffer(render_command_buffers[image_index]);
  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = RenderPass::present_pass();
  renderPassInfo.framebuffer = framebuffers[image_index];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = VkExtent2D {uint32_t(_viewport.width), uint32_t(_viewport.height)};
  VkClearValue clear_value {0.f, 0.f, 0.f, 1.f};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clear_value;

  vkCmdBeginRenderPass(cmd_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void Vulkan::end_present_pass() {
  VkCommandBuffer cmd_buffer(render_command_buffers[image_index]);
  vkCmdEndRenderPass(cmd_buffer);
}
VkCommandBuffer Vulkan::begin_command_buffer() {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = command_pool[TaskSystem::fiber_id()];
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void Vulkan::end_command_buffer(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  {
    L_SCOPED_LOCK(queue_lock);
    VkFence fence(command_fence[TaskSystem::fiber_id()]);
    vkQueueSubmit(queue, 1, &submitInfo, fence);
    L_SCOPE_MARKER("Command buffer fence");
    TaskSystem::yield_until([](void* fence){
      return vkGetFenceStatus(Vulkan::device(), *(VkFence*)fence)==VK_SUCCESS;
    }, &fence);
    vkResetFences(_device, 1, &fence);
  }

  vkFreeCommandBuffers(_device, command_pool[TaskSystem::fiber_id()], 1, &commandBuffer);
}

bool Vulkan::find_buffer(VkDeviceSize size, VkBufferUsageFlagBits usage, VkBuffer& buffer, VkDeviceMemory& memory) {
  for(uintptr_t i(0); i<free_buffers.size(); i++) {
    const FlyingBuffer& flying_buffer(free_buffers[i]);
    if(flying_buffer.size==size && flying_buffer.usage==usage) {
      buffer = flying_buffer.buffer;
      memory = flying_buffer.memory;
      free_buffers.erase_fast(i);
      return true;
    }
  }
  return false;
}
void Vulkan::destroy_buffer(VkDeviceSize size, VkBufferUsageFlagBits usage, VkBuffer buffer, VkDeviceMemory memory) {
  used_buffers.push(FlyingBuffer {buffer, memory, size, usage});
}
bool Vulkan::find_desc_set(VkPipeline pipeline, VkDescriptorSet& set) {
  for(uintptr_t i(0); i<free_sets.size(); i++) {
    const FlyingSet& flying_set(free_sets[i]);
    if(flying_set.pipeline==pipeline) {
      set = flying_set.set;
      free_sets.erase_fast(i);
      return true;
    }
  }
  return false;
}
void Vulkan::destroy_desc_set(VkPipeline pipeline, VkDescriptorSet set) {
  used_sets.push(FlyingSet {set, pipeline});
}
void Vulkan::destroy_framebuffer(VkFramebuffer framebuffer) {
  garbage_framebuffers.push(framebuffer);
}
void Vulkan::destroy_image(VkImage image, VkDeviceMemory memory){
  garbage_images.push(GarbageImage {image, memory});
}

VkDevice Vulkan::device() { return _device; }
VkDescriptorPool Vulkan::descriptor_pool() { return _descriptor_pool; }
VkSampler Vulkan::sampler() { return _sampler; }
