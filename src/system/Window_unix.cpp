#include "Window.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include "System.h"
#include "../stream/CFileStream.h"

#include "../rendering/Vulkan.h"

using namespace L;
using L::Window;

static bool win_opened(false);
Display* xdisplay;
::Window xwindow;

void Window::open(const char* title, int width, int height, int flags) {
  if(opened()) return;
  _width = width;
  _height = height;
  _flags = flags;

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
        if(_width!=gwa.width || _height!=gwa.height) {
          e.type = Event::Resize;
          _width = e.x = gwa.width;
          _height = e.y = gwa.height;
        }
        break;
      case MotionNotify:
        e.type = Event::MouseMove;
        e.x = xev.xmotion.x;
        e.y = xev.xmotion.y;
        e.x -= _mousePos.x();
        e.y -= _mousePos.y();
        if(_flags&loopcursor && (e.x!=0 || e.y!=0)) {
          XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, 128, 128);
          _mousePos = Vector2i(128, 128);
        } else {
          _mousePos += Vector2i(e.x, e.y);
        }
        break;
      case ButtonPress:
      case ButtonRelease: {
          e.type = (xev.type==ButtonPress) ? Event::ButtonDown : Event::ButtonUp;
          if(xev.xbutton.button==Button1) e.button = Event::LeftButton;
          else if(xev.xbutton.button==Button2) e.button = Event::RightButton;
          else if(xev.xbutton.button==Button3) e.button = Event::MiddleButton;
        }
        break;
      case KeyPress:
      case KeyRelease: {
          e.type = (xev.type==KeyPress) ? Event::ButtonDown : Event::ButtonUp;
          const KeySym key_sym(XLookupKeysym(&xev.xkey, 0));
          switch(key_sym) {
            #define MAP(xk,lb) case xk: e.button = Event::lb; break;
            MAP(XK_BackSpace,Backspace)
            MAP(XK_Tab,Tab)
            MAP(XK_Return,Enter)
            MAP(XK_Shift_L,Shift)
            MAP(XK_Shift_R,Shift)
            MAP(XK_Control_L,Ctrl)
            MAP(XK_Control_R,Ctrl)
            MAP(XK_Alt_L,Alt)
            MAP(XK_Alt_R,Alt)
            MAP(XK_Pause,Pause)
            MAP(XK_Caps_Lock,Caps)
            MAP(XK_Escape,Escape)
            MAP(XK_KP_Space,Space)
            MAP(XK_Left,Left) MAP(XK_Right,Right) MAP(XK_Up,Up) MAP(XK_Down,Down)
            MAP(XK_Num_Lock,NumLock)
            MAP(XK_KP_1,Num1) MAP(XK_KP_2,Num2) MAP(XK_KP_3,Num3)
            MAP(XK_KP_4,Num4) MAP(XK_KP_5,Num5) MAP(XK_KP_6,Num6)
            MAP(XK_KP_7,Num7) MAP(XK_KP_8,Num8) MAP(XK_KP_9,Num9)
            MAP(XK_KP_0,Num0)
            MAP(XK_F1,F1) MAP(XK_F2,F2) MAP(XK_F3,F3) MAP(XK_F4,F4)
            MAP(XK_F5,F5) MAP(XK_F6,F6) MAP(XK_F7,F7) MAP(XK_F8,F8)
            MAP(XK_F9,F9) MAP(XK_F10,F10) MAP(XK_F11,F11) MAP(XK_F12,F12)
            #undef MAP
            default:
              if(key_sym>=XK_a && key_sym<=XK_z)
                e.button = Event::Button((key_sym-XK_a)+'A');
              else if(key_sym>=XK_A && key_sym<=XK_Z)
                e.button = Event::Button((key_sym-XK_Z)+'A');
              break;
          }
        }
        break;
      case ClientMessage: // It's the close operation
        close();
        break;
    }
    #undef None // That's awkward
    if(e.type!=Event::None) {
      if(e.type==Event::ButtonDown)
        _buttonstate[e.button] = true;
      else if(e.type==Event::ButtonUp)
        _buttonstate[e.button] = false;
      _events.push(e);
    }
  }
  return opened();
}

void Window::title(const char* str) {
  if(!opened()) return;
}
void Window::resize(int width,int height) {
  if(!opened()) return;
  _width = width;
  _height = height;
}

void Window::mousePosition(const Vector2i& p){
}
