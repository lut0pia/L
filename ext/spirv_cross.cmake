set(SPIRV_CROSS_RLS_LIBRARIES
  lib/spirv-cross-c
  lib/spirv-cross-core
  lib/spirv-cross-cpp
  lib/spirv-cross-glsl
  lib/spirv-cross-hlsl
  lib/spirv-cross-msl
  lib/spirv-cross-reflect
)
set(SPIRV_CROSS_DBG_LIBRARIES ${SPIRV_CROSS_RLS_LIBRARIES})

if(MSVC)
  list(TRANSFORM SPIRV_CROSS_DBG_LIBRARIES APPEND "d")
endif()

add_external(
  spirv_cross
  CMAKE
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
  GIT_TAG 8891bd35120ca91c252a66ccfdc3f9a9d03c70cd # 2020-09-17
  INCLUDE_DIRS include
  DBG_LIBRARIES ${SPIRV_CROSS_DBG_LIBRARIES}
  RLS_LIBRARIES ${SPIRV_CROSS_RLS_LIBRARIES}
)
