add_module(
  imgui_console
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/imgui_console.cpp
  MOD_DEPENDENCIES imgui
)
