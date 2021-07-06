add_module(
  stb_truetype
  CONDITION ${DEV_DBG}
  EXT_DEPENDENCIES stb
  MOD_DEPENDENCIES font_tools
)
