#include "Vulkan.h"

#include <vulkan/vulkan.h>
#include <windows.h>

#include "../dev/debug.h"

extern HWND hWND;

namespace L {
  namespace Vulkan {
    extern VkInstance instance;
    void create_surface(VkSurfaceKHR* surface) {
      VkWin32SurfaceCreateInfoKHR create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
      create_info.hwnd = hWND;
      create_info.hinstance = GetModuleHandle(nullptr);
      L_VK_CHECKED(vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface));
    }
    const char* extra_extension() {
      return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    }
  }
}
