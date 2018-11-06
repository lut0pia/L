#define VK_USE_PLATFORM_XLIB_KHR

#include <L/src/engine/Engine.h>
#include <L/src/rendering/Vulkan.h>
#include <L/src/system/Window.h>

#include <X11/X.h>
#include <X11/Xlib.h>

using namespace L;
using L::Window;

static XWindow* instance(nullptr);

class XWindow : public Window {
protected:
  Display* _xdisplay;
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
          if(_width!=gwa.width || _height!=gwa.height) {
            e.type = Event::Resize;
            _width = e.x = gwa.width;
            _height = e.y = gwa.height;
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
#undef None // That's awkward
      if(e.type!=Event::Type::None) {
        _events.push(e);
      }
    }
  }
  static void static_update() {
    instance->update();
  }

  void open(const char* title, uint32_t width, uint32_t height, uint32_t flags) override {
    L_SCOPE_MARKER("Window::open");
    if(opened()) return;
    _width = width;
    _height = height;
    _flags = flags;

    if((_xdisplay = XOpenDisplay(nullptr)) == nullptr)
      error("Cannot open X server display.");

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

    if(flags&nocursor) {
      const char pixmap_data(0);
      Pixmap pixmap(XCreateBitmapFromData(_xdisplay, _xwindow, &pixmap_data, 1, 1));
      XColor color;
      Cursor cursor(XCreatePixmapCursor(_xdisplay, pixmap, pixmap, &color, &color, 0, 0));
      XDefineCursor(_xdisplay, _xwindow, cursor);
    }
    _opened = true;

    Vulkan::init();
  }
  void close() override {
    L_ASSERT(_opened);
    XDestroyWindow(_xdisplay, _xwindow);
    XCloseDisplay(_xdisplay);
    _opened = false;
  }

  void title(const char* str) override {}
  void resize(uint32_t width, uint32_t height) override {}
  void create_vulkan_surface(VkInstance instance, VkSurfaceKHR* surface) override {
    VkXlibSurfaceCreateInfoKHR create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    create_info.dpy = _xdisplay;
    create_info.window = _xwindow;
    L_VK_CHECKED(vkCreateXlibSurfaceKHR(instance, &create_info, nullptr, surface));
  }
  const char* extra_vulkan_extension() override {
    return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
  }
};

void xlib_module_init() {
  instance = Memory::new_type<XWindow>();
  Engine::add_parallel_update([]() {
    XWindow::update();
  });
}
