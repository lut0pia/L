add_module(
  rawinput
  CONDITION $<PLATFORM_ID:Windows>
  LIBRARIES hid
)
