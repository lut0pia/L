add_module(
  xinput_win
  CONDITION $<PLATFORM_ID:Windows>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/xinput_win.cpp
)
