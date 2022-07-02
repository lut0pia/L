add_module(
  test_compression
  CONDITION ${DEV_DBG}
  MOD_DEPENDENCIES lz zlib
)
