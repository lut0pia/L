set(GLSLANG_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/glslang)

ExternalProject_Add(
  ext_glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG 8db9eccc0baf30c9d22c496ab28db0fe1e4e97c5 # v8.13.3559
  GIT_SHALLOW true
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GLSLANG_INSTALL_DIR} -DCMAKE_BUILD_TYPE=$<IF:$<CONFIG:Debug>,DEBUG,RELEASE>
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<IF:$<CONFIG:Debug>,Debug,Release>
  INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<IF:$<CONFIG:Debug>,Debug,Release>
)
ExternalProject_Get_Property(ext_glslang SOURCE_DIR)
ExternalProject_Get_Property(ext_glslang BINARY_DIR)

add_module(
  glsl
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/glsl.cpp
  DEPENDENCIES ext_glslang
  INCLUDE_DIRS ${GLSLANG_INSTALL_DIR}/include
  DBG_LIBRARIES
    ${GLSLANG_INSTALL_DIR}/lib/glslangd
    ${GLSLANG_INSTALL_DIR}/lib/OSDependentd
    ${GLSLANG_INSTALL_DIR}/lib/HLSLd
    ${GLSLANG_INSTALL_DIR}/lib/OGLCompilerd
    ${GLSLANG_INSTALL_DIR}/lib/SPIRVd
  RLS_LIBRARIES
    ${GLSLANG_INSTALL_DIR}/lib/glslang
    ${GLSLANG_INSTALL_DIR}/lib/OSDependent
    ${GLSLANG_INSTALL_DIR}/lib/HLSL
    ${GLSLANG_INSTALL_DIR}/lib/OGLCompiler
    ${GLSLANG_INSTALL_DIR}/lib/SPIRV
)
