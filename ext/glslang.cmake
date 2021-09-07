set(GLSLANG_RLS_LIBRARIES
  lib/glslang
  lib/OSDependent
  lib/HLSL
  lib/OGLCompiler
  lib/SPIRV
)
set(GLSLANG_DBG_LIBRARIES ${GLSLANG_RLS_LIBRARIES})

if(MSVC)
  list(TRANSFORM GLSLANG_DBG_LIBRARIES APPEND "d")
endif()

add_external(
  glslang
  CMAKE
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG bcf6a2430e99e8fc24f9f266e99316905e6d5134 # v8.13.3743
  INCLUDE_DIRS include
  DBG_LIBRARIES ${GLSLANG_DBG_LIBRARIES}
  RLS_LIBRARIES ${GLSLANG_RLS_LIBRARIES}
)