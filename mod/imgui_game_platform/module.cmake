add_module(
  imgui_game_platform
  CONDITION ${DEV_DBG}
  MOD_DEPENDENCIES imgui
  EXT_DEPENDENCIES imgui
)
