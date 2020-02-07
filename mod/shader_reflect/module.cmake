add_module(
  shader_reflect
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/shader_reflect.cpp
)
