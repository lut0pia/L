ExternalProject_Add(
  ext_glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG 8db9eccc0baf30c9d22c496ab28db0fe1e4e97c5 # v8.13.3559
  GIT_SHALLOW true
  INSTALL_COMMAND ""
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<IF:$<CONFIG:Debug>,Debug,Release>
)
ExternalProject_Get_Property(ext_glslang SOURCE_DIR)
ExternalProject_Get_Property(ext_glslang BINARY_DIR)

add_module(
  glsl
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/glsl.cpp
  DEPENDENCIES ext_glslang
  DBG_LIBRARIES glslangd OSDependentd HLSLd OGLCompilerd SPIRVd
  RLS_LIBRARIES glslang OSDependent HLSL OGLCompiler SPIRV
  INCLUDE_DIRS ${SOURCE_DIR}
  LINK_DIRS
    ${BINARY_DIR}/glslang/Debug
    ${BINARY_DIR}/glslang/Release
    ${BINARY_DIR}/glslang/OSDependent/Linux/Debug
    ${BINARY_DIR}/glslang/OSDependent/Linux/Release
    ${BINARY_DIR}/glslang/OSDependent/Windows/Debug
    ${BINARY_DIR}/glslang/OSDependent/Windows/Release
    ${BINARY_DIR}/hlsl/Debug
    ${BINARY_DIR}/hlsl/Release
    ${BINARY_DIR}/OGLCompilersDLL/Debug
    ${BINARY_DIR}/OGLCompilersDLL/Release
    ${BINARY_DIR}/SPIRV/Debug
    ${BINARY_DIR}/SPIRV/Release
)
