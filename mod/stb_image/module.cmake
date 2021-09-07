add_module(
  stb_image
  DEFAULT_ENABLED ON
  CONDITION ${DEV_DBG}
  EXT_DEPENDENCIES stb
)
