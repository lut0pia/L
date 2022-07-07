set(L_STEAMWORKS_SDK "" CACHE PATH "Path to the wanted Steamworks SDK")

if("${L_STEAMWORKS_SDK}" STREQUAL "")
  message("Steamworks SDK not set up")
  return()
endif()

if(NOT EXISTS "${L_STEAMWORKS_SDK}/public/steam/steam_api.h")
  message("Invalid Steamworks SDK location (cannot find steam_api.h)")
  return()
endif()

add_module(
  steamworks
  SYSTEM_INCLUDE_DIRS ${L_STEAMWORKS_SDK}/public
  LIBRARIES ${L_STEAMWORKS_SDK}/redistributable_bin/win64/steam_api64.lib
)
