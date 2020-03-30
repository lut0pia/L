add_module(
  pixel_font
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/pixel_font.cpp
  MOD_DEPENDENCIES font_tools
)
