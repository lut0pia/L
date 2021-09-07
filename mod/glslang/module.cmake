add_module(
  glslang
  DEFAULT_ENABLED ON
  CONDITION ${DEV_DBG}
  EXT_DEPENDENCIES glslang
)
