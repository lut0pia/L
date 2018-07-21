#define VK_USE_PLATFORM_XLIB_KHR
#include "Vulkan.h"

#include <vulkan/vulkan.h>

#include "../dev/debug.h"

extern Display* xdisplay;
extern ::Window xwindow;

namespace L {
  namespace Vulkan {
    extern VkInstance instance;
    void create_surface(VkSurfaceKHR* surface) {
      VkXlibSurfaceCreateInfoKHR create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
      create_info.dpy = xdisplay;
      create_info.window = xwindow;
      L_VK_CHECKED(vkCreateXlibSurfaceKHR(instance, &create_info, nullptr, surface));
    }
    const char* extra_extension() {
      return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
    }
  }
}
