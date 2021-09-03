#include <L/src/engine/Engine.h>
#include <L/src/rendering/Renderer.h>
#include <L/src/system/Window.h>

#include "xlib.h"

static class XWindow* instance(nullptr);

L::Symbol xlib_window_type("xlib");

class XWindow : public L::Window {
protected:
  ::Display* _xdisplay;
  ::Window _xwindow;

public:
  void update() {
    L_SCOPE_MARKER("Window update");
    XEvent xev;
    XWindowAttributes gwa;
    while(opened() && XPending(_xdisplay)) {
      XNextEvent(_xdisplay, &xev);
      Window::Event e {};
      switch(xev.type) {
        case Expose:
          XGetWindowAttributes(_xdisplay, _xwindow, &gwa);
          if(_width != uint32_t(gwa.width) || _height != uint32_t(gwa.height)) {
            e.type = Event::Type::Resize;
            _width = e.coords.x = gwa.width;
            _height = e.coords.y = gwa.height;
          }
          break;
        case MotionNotify:
          _cursor_x = xev.xmotion.x;
          _cursor_y = xev.xmotion.y;
          break;
        case ClientMessage: // It's the close operation
          close();
          break;
      }
      if(e.type!=Event::Type::None) {
        _events.push(e);
      }
    }
  }

  void open(const char* title, uint32_t width, uint32_t height, uint32_t flags) override {
    L_SCOPE_MARKER("Window::open");
    if(opened()) return;
    _width = width;
    _height = height;
    _flags = flags;

    if((_xdisplay = XOpenDisplay(nullptr)) == nullptr) {
      L::error("Cannot open X server display.");
    }

    { // Create window
      ::Window root = DefaultRootWindow(_xdisplay);
      int scr = DefaultScreen(_xdisplay);
      int depth = DefaultDepth(_xdisplay, scr);
      Visual* visual = DefaultVisual(_xdisplay, scr);
      XSetWindowAttributes swa {};
      swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
      _xwindow = XCreateWindow(_xdisplay, root, 0, 0, width, height, 0, depth, InputOutput, visual, CWColormap | CWEventMask, &swa);
    }
    { // Activate window close events
      Atom wm_delete_window(XInternAtom(_xdisplay, "WM_DELETE_WINDOW", 0));
      XSetWMProtocols(_xdisplay, _xwindow, &wm_delete_window, 1);
    }
    XMapWindow(_xdisplay, _xwindow);
    XStoreName(_xdisplay, _xwindow, title);

    if(flags & nocursor) {
      const char pixmap_data(0);
      Pixmap pixmap(XCreateBitmapFromData(_xdisplay, _xwindow, &pixmap_data, 1, 1));
      XColor color;
      Cursor cursor(XCreatePixmapCursor(_xdisplay, pixmap, pixmap, &color, &color, 0, 0));
      XDefineCursor(_xdisplay, _xwindow, cursor);
    }
    _opened = true;

    XlibWindowData window_data;
    window_data.type = xlib_window_type;
    window_data.display = _xdisplay;
    window_data.window = _xwindow;

    L::Renderer::get()->init(&window_data);
  }
  void close() override {
    L_ASSERT(_opened);
    XDestroyWindow(_xdisplay, _xwindow);
    XCloseDisplay(_xdisplay);
    _opened = false;
  }

  void title(const char*) override {
    L::warning("Setting window title is unsupported for X windows");
  }
  void resize(uint32_t, uint32_t) override {
    L::warning("Resizing window is unsupported for X windows");
  }
};

void xlib_module_init() {
  instance = L::Memory::new_type<XWindow>();
  L::Engine::add_parallel_update([]() {
    instance->update();
  });
}
