#include "Window.h"

#include <X11/X.h>
#include <X11/Xlib.h>

#include "../container/Queue.h"
#include "../rendering/Vulkan.h"

using namespace L;
using L::Window;

namespace L {
  extern Queue<64, Window::Event> window_events;
  extern int window_width, window_height, window_flags, mouse_x, mouse_y;
}

static bool win_opened(false);
Display* xdisplay;
::Window xwindow;

void Window::open(const char* title, uint32_t width, uint32_t height, uint32_t flags) {
  if(opened()) return;
  window_width = width;
  window_height = height;
  window_flags = flags;

  if((xdisplay = XOpenDisplay(nullptr)) == nullptr)
    error("Cannot open X server display.");

  {
    ::Window root = DefaultRootWindow(xdisplay);
    int scr = DefaultScreen(xdisplay);
    int depth = DefaultDepth(xdisplay, scr);
    Visual* visual = DefaultVisual(xdisplay, scr);
    XSetWindowAttributes swa {};    
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    xwindow = XCreateWindow(xdisplay,root,0,0,width,height,0,depth,InputOutput,visual,CWColormap | CWEventMask,&swa);
    //Atom delWindow = XInternAtom(dpy,"WM_DELETE_W aINDOW",0); // This is for the window close operation
    //XSetWMProtocols(dpy,win,&delWindow,1);
  }
  XMapWindow(xdisplay,xwindow);
  XStoreName(xdisplay,xwindow,title);

  if(flags&nocursor) {
    const char pixmap_data(0);
    Pixmap pixmap(XCreateBitmapFromData(xdisplay, xwindow, &pixmap_data, 1, 1));
    XColor color;
    Cursor cursor(XCreatePixmapCursor(xdisplay, pixmap, pixmap, &color, &color, 0, 0));
    XDefineCursor(xdisplay, xwindow, cursor);
  }
  win_opened = true;

  Vulkan::init();
}

void Window::close() {
  if(!opened()) return;
  XDestroyWindow(xdisplay,xwindow);
  XCloseDisplay(xdisplay);
  win_opened = false;
}
bool Window::opened() {
  return win_opened;
}
bool Window::loop() {
  XEvent xev;
  XWindowAttributes gwa;
  while(opened() && XPending(xdisplay)) {
    XNextEvent(xdisplay,&xev);
    Window::Event e;
    switch(xev.type) {
      case Expose:
        XGetWindowAttributes(xdisplay, xwindow, &gwa);
        if(window_width!=gwa.width || window_height!=gwa.height) {
          e.type = Event::Resize;
          window_width = e.x = gwa.width;
          window_height = e.y = gwa.height;
        }
        break;
      case MotionNotify:
        L::mouse_x = xev.xmotion.x;
        L::mouse_y = xev.xmotion.y;
        break;
      case ClientMessage: // It's the close operation
        close();
        break;
    }
    #undef None // That's awkward
    if(e.type!=Event::None) {
      window_events.push(e);
    }
  }
  return opened();
}

void Window::title(const char* str) {

}
void Window::resize(uint32_t width, uint32_t height) {

}
