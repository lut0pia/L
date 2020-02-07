add_module(
  stb_image
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/stb_image.cpp
)
