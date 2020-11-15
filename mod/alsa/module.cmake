find_path(ALSA_PATHS alsa/asoundlib.h)
if(NOT ALSA_PATHS)
  message("-- Could NOT find ALSA")
  return()
endif()

add_module(
  alsa
  CONDITION $<PLATFORM_ID:Linux>
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/alsa.cpp
  LIBRARIES asound
)
