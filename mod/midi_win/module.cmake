add_module(
  midi_win
  CONDITION $<PLATFORM_ID:Windows>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/midi_win.cpp
  LIBRARIES Winmm
)
