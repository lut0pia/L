add_module(
  script_optimize
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/script_optimize.cpp
)
