add_module(
  imgui_resources
  CONDITION ${DEV_DBG}
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/imgui_resources.cpp
  MOD_DEPENDENCIES imgui
)
