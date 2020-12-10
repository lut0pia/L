find_path(ALSA_PATHS alsa/asoundlib.h)
if(NOT ALSA_PATHS)
  message("Could NOT find ALSA")
  unset(ALSA_PATHS CACHE)
  return()
else()
  unset(ALSA_PATHS CACHE)
endif()

add_module(
  alsa
  CONDITION $<PLATFORM_ID:Linux>
  LIBRARIES asound
)
