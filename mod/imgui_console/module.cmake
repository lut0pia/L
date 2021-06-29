add_module(
  imgui_console
  CONDITION ${DEV_DBG}
  MOD_DEPENDENCIES imgui
  EXT_DEPENDENCIES imgui
)
