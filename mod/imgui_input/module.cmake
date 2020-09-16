add_module(
  imgui_input
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/imgui_input.cpp
  MOD_DEPENDENCIES imgui
)
