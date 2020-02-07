add_module(
  audio_win
  CONDITION $<PLATFORM_ID:Windows>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/audio_win.cpp
)
