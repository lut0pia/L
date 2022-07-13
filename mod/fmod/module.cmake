set(L_FMOD_SDK "" CACHE PATH "Path to the wanted FMOD SDK")

if("${L_FMOD_SDK}" STREQUAL "")
  message("FMOD SDK not set up")
  return()
endif()

if(NOT EXISTS "${L_FMOD_SDK}/api/core/inc/fmod.h")
  message("Invalid FMOD SDK location (cannot find fmod.h)")
  return()
endif()

add_module(
  fmod
  INCLUDE_DIRS
    ${L_FMOD_SDK}/api/core/inc
    ${L_FMOD_SDK}/api/studio/inc
  LIBRARIES
    ${L_FMOD_SDK}/api/core/lib/x64/fmod_vc.lib
    ${L_FMOD_SDK}/api/studio/lib/x64/fmodstudio_vc.lib
)
