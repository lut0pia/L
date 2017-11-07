#include "Window.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "System.h"
#include "../gl/GL.h"
#include "../stream/CFileStream.h"

using namespace L;
using L::Window;

bool                    winOpened(false);
Display                 *dpy;
::Window                root;
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
::Window                win;
GLXContext              glc;
XWindowAttributes       gwa;


void Window::open(const char* title, int width, int height, int flags) {
  if(opened()) return;
  _width = width;
  _height = height;
  _flags = flags;

  if((dpy = XOpenDisplay(nullptr)) == nullptr)
    error("Cannot open X server display.");

  root = DefaultRootWindow(dpy);
  GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  if((vi = glXChooseVisual(dpy,0,att)) == nullptr)
    error("No appropriate visual found for X server.");
  cmap = XCreateColormap(dpy,root,vi->visual,AllocNone);
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
  win = XCreateWindow(dpy,root,0,0,width,height,0,vi->depth,InputOutput,vi->visual,CWColormap | CWEventMask,&swa);
  Atom delWindow = XInternAtom(dpy,"WM_DELETE_WINDOW",0); // This is for the window close operation
  XSetWMProtocols(dpy,win,&delWindow,1);
  XMapWindow(dpy,win);
  XStoreName(dpy,win,title);
  glc = glXCreateContext(dpy,vi,nullptr,GL_TRUE);
  glXMakeCurrent(dpy,win,glc);
  if(flags&nocursor) {
    const char pixmap_data(0);
    Pixmap pixmap(XCreateBitmapFromData(dpy, win, &pixmap_data, 1, 1));
    XColor color;
    Cursor cursor(XCreatePixmapCursor(dpy, pixmap, pixmap, &color, &color, 0, 0));
    XDefineCursor(dpy, win, cursor);
  }
  winOpened = true;

  GL::init();
}

void Window::close() {
  if(!opened()) return;
  glXMakeCurrent(dpy,None,nullptr);
  glXDestroyContext(dpy,glc);
  XDestroyWindow(dpy,win);
  XCloseDisplay(dpy);
  winOpened = false;
}
bool Window::opened() {
  return (winOpened);
}
bool Window::loop() {
  XEvent xev;
  while(opened() && XPending(dpy)) {
    XNextEvent(dpy,&xev);
    Window::Event e;
    switch(xev.type) {
      case Expose:
        XGetWindowAttributes(dpy, win, &gwa);
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
          XWarpPointer(dpy, None, win, 0, 0, 0, 0, 128, 128);
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
void Window::swapBuffers() {
  if(!opened()) return;
  glXSwapBuffers(dpy,win);
}
void Window::draw(const Bitmap& bmp) {
  if(!opened()) return;
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
