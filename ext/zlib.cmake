if(UNIX)
  set(ZLIB_RLS_LIBRARIES lib/z)
else()
  set(ZLIB_RLS_LIBRARIES lib/zlibstatic)
endif()

set(ZLIB_DBG_LIBRARIES ${ZLIB_RLS_LIBRARIES})

if(MSVC)
  list(TRANSFORM ZLIB_DBG_LIBRARIES APPEND "d")
endif()

add_external(
  zlib
  CMAKE
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG cacf7f1d4e3d44d871b605da3b647f07d718623f # v1.2.11
  INCLUDE_DIRS include
  DBG_LIBRARIES ${ZLIB_DBG_LIBRARIES}
  RLS_LIBRARIES ${ZLIB_RLS_LIBRARIES}
)
