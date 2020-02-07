add_module(
  alsa
  CONDITION $<PLATFORM_ID:Linux>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/alsa.cpp
  LIBRARIES asound
)
