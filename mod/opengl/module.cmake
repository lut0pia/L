find_package(OpenGL)
if(NOT OpenGL_OpenGL_FOUND)
  return()
endif()

set(SPIRV_CROSS_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/spirv_cross)
set(SPIRV_CROSS_RLS_LIBRARIES
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-c
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-core
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-cpp
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-glsl
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-hlsl
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-msl
  ${SPIRV_CROSS_INSTALL_DIR}/lib/spirv-cross-reflect
)
set(SPIRV_CROSS_DBG_LIBRARIES ${SPIRV_CROSS_RLS_LIBRARIES})

if(MSVC)
  list(TRANSFORM SPIRV_CROSS_DBG_LIBRARIES APPEND "d")
endif()

ExternalProject_Add(
  ext_spirv_cross
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
  GIT_TAG 8891bd35120ca91c252a66ccfdc3f9a9d03c70cd # 2020-09-17
  GIT_SHALLOW true
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${SPIRV_CROSS_INSTALL_DIR} -DCMAKE_BUILD_TYPE=$<IF:$<CONFIG:Debug>,DEBUG,RELEASE>
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<IF:$<CONFIG:Debug>,Debug,Release>
  INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<IF:$<CONFIG:Debug>,Debug,Release>
)
ExternalProject_Get_Property(ext_spirv_cross SOURCE_DIR)
ExternalProject_Get_Property(ext_spirv_cross BINARY_DIR)

file(GLOB OPENGL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_module(
  opengl
  SOURCES ${OPENGL_SOURCES}
  INCLUDE_DIRS
	${CMAKE_CURRENT_LIST_DIR}
	${OPENGL_INCLUDE_DIR}
	${SPIRV_CROSS_INSTALL_DIR}/include
  LIBRARIES ${OPENGL_gl_LIBRARY}
  DBG_LIBRARIES ${SPIRV_CROSS_DBG_LIBRARIES}
  RLS_LIBRARIES ${SPIRV_CROSS_RLS_LIBRARIES}
  DEPENDENCIES ext_spirv_cross
)
