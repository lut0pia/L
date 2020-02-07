add_module(
  rawinput
  CONDITION $<PLATFORM_ID:Windows>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/rawinput.cpp
  LIBRARIES hid
)
