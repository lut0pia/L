if(NOT DEFINED ENV{WWISESDK})
  message("Could NOT find Wwise")
  return()
endif()

set(WWISE_RLS_LIBRARIES
  AkSoundEngine
  AkMemoryMgr
  AkStreamMgr
  AkMusicEngine
  AkSpatialAudio
)
set(WWISE_DBG_LIBRARIES ${WWISE_RLS_LIBRARIES})
list(APPEND WWISE_DBG_LIBRARIES CommunicationCentral)

set(WWISE_ARCH x64_vc160)

list(TRANSFORM WWISE_DBG_LIBRARIES PREPEND $ENV{WWISESDK}/${WWISE_ARCH}/Debug/lib/)
list(TRANSFORM WWISE_RLS_LIBRARIES PREPEND $ENV{WWISESDK}/${WWISE_ARCH}/Release/lib/)

add_module(
  wwise
  INCLUDE_DIRS
    $ENV{WWISESDK}/include
    $ENV{WWISESDK}/samples/SoundEngine/Common
    $ENV{WWISESDK}/samples/SoundEngine/Win32
  DBG_LIBRARIES ${WWISE_DBG_LIBRARIES}
  RLS_LIBRARIES ${WWISE_RLS_LIBRARIES}
)
