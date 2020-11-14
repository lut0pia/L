find_package(Vulkan)
if(NOT Vulkan_FOUND)
  return()
endif()

include_directories(${Vulkan_INCLUDE_DIRS})
link_libraries(${Vulkan_LIBRARIES})

file(GLOB VULKAN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_module(
  vulkan
  SOURCES ${VULKAN_SOURCES}
)
