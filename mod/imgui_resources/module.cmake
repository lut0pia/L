add_module(
  imgui_resources
  CONDITION ${DEV_DBG}
  MOD_DEPENDENCIES imgui
  EXT_DEPENDENCIES imgui
)
