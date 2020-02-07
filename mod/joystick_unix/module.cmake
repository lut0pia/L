add_module(
  joystick_unix
  CONDITION $<PLATFORM_ID:Linux>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/joystick_unix.cpp
)
