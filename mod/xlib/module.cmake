add_module(
  xlib
  CONDITION $<PLATFORM_ID:Linux>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/xlib.cpp
)
