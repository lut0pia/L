#pragma once

#include <L/src/rendering/Renderer.h>

#include <X11/X.h>
#include <X11/Xlib.h>

extern L::Symbol xlib_window_type;

struct XlibWindowData : public L::GenericWindowData {
  ::Display* display;
  ::Window window;
};
