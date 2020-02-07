add_module(
  wavefront_obj
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/wavefront_obj.cpp
)
