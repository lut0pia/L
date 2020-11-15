find_package(Vulkan)
if(NOT Vulkan_FOUND)
  return()
endif()

file(GLOB VULKAN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_module(
  vulkan
  SOURCES ${VULKAN_SOURCES}
  INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS}
  LIBRARIES ${Vulkan_LIBRARIES}
)
