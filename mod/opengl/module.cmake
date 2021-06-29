set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL)
if(NOT OpenGL_OpenGL_FOUND)
  return()
endif()

set(GL_LIBRARIES ${OPENGL_gl_LIBRARY})
if(EXISTS ${OPENGL_opengl_LIBRARY})
  list(APPEND GL_LIBRARIES ${OPENGL_opengl_LIBRARY})
endif()
if(OpenGL_GLX_FOUND)
  list(APPEND GL_LIBRARIES ${OPENGL_glx_LIBRARY})
endif()

add_module(
  opengl
  INCLUDE_DIRS ${OPENGL_INCLUDE_DIR}
  LIBRARIES ${GL_LIBRARIES}
  OPT_MOD_DEPENDENCIES win32 xlib
  EXT_DEPENDENCIES spirv_cross opengl_registry egl_registry
)
