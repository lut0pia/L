add_module(
  xinput_unix
  CONDITION $<PLATFORM_ID:Linux>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/xinput_unix.cpp
  LIBRARIES dl
)
