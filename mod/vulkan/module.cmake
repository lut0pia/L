find_package(Vulkan)
if(NOT Vulkan_FOUND)
  return()
endif()

add_module(
  vulkan
  INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS}
  LIBRARIES ${Vulkan_LIBRARIES}
  OPT_MOD_DEPENDENCIES win32 xlib
)
