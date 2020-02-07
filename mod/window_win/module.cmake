add_module(
  window_win
  CONDITION $<PLATFORM_ID:Windows>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/window_win.cpp
)
