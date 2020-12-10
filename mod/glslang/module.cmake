set(GLSLANG_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/glslang)
set(GLSLANG_RLS_LIBRARIES
  ${GLSLANG_INSTALL_DIR}/lib/glslang
  ${GLSLANG_INSTALL_DIR}/lib/OSDependent
  ${GLSLANG_INSTALL_DIR}/lib/HLSL
  ${GLSLANG_INSTALL_DIR}/lib/OGLCompiler
  ${GLSLANG_INSTALL_DIR}/lib/SPIRV
)
set(GLSLANG_DBG_LIBRARIES ${GLSLANG_RLS_LIBRARIES})

if(MSVC)
  list(TRANSFORM GLSLANG_DBG_LIBRARIES APPEND "d")
endif()

ExternalProject_Add(
  ext_glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG bcf6a2430e99e8fc24f9f266e99316905e6d5134 # v8.13.3743
  GIT_SHALLOW true
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GLSLANG_INSTALL_DIR} -DCMAKE_BUILD_TYPE=$<IF:$<CONFIG:Debug>,DEBUG,RELEASE>
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<IF:$<CONFIG:Debug>,Debug,Release>
  INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<IF:$<CONFIG:Debug>,Debug,Release>
)
ExternalProject_Get_Property(ext_glslang SOURCE_DIR)
ExternalProject_Get_Property(ext_glslang BINARY_DIR)

add_module(
  glslang
  CONDITION ${DEV_DBG}
  DEPENDENCIES ext_glslang
  INCLUDE_DIRS ${GLSLANG_INSTALL_DIR}/include
  DBG_LIBRARIES ${GLSLANG_DBG_LIBRARIES}
  RLS_LIBRARIES ${GLSLANG_RLS_LIBRARIES}
)
