add_module(
  blue_noise
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/blue_noise.cpp
)
