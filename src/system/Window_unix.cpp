#include "Window.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "System.h"
#include "../gl/GL.h"

using namespace L;
using L::Window;

bool                    winOpened(false);
Display                 *dpy;
::Window                root;
GLint                   att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
::Window                win;
GLXContext              glc;
XWindowAttributes       gwa;
void eventTranslate(const XEvent& xev) {
  Window::Event e;
  switch(xev.type) {
    case MotionNotify:
      e.type = Window::Event::MouseMove;
      e.x = xev.xmotion.x;
      e.y = xev.xmotion.y;
      break;
    case KeyPress:
      break;
    case ResizeRequest:
      e.type = Window::Event::Resize;
      e.x = xev.xresizerequest.width;
      e.y = xev.xresizerequest.height;
      XResizeWindow(dpy,win,e.x,e.y);
      break;
    case ClientMessage: // It's the close operation
      Window::close();
      break;
    default:
      return;
      break;
  }
  Window::_events.push(e);
}

void Window::open(const char* title,int width,int height,int flags) {
  if(opened()) return;
  _width = width;
  _height = height;
  _flags = flags;

  if((dpy = XOpenDisplay(nullptr)) == nullptr)
    L_ERROR("Cannot open X server display.");
  root = DefaultRootWindow(dpy);
  if((vi = glXChooseVisual(dpy,0,att)) == nullptr)
    L_ERROR("No appropriate visual found for X server.");
  cmap = XCreateColormap(dpy,root,vi->visual,AllocNone);
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;
  win = XCreateWindow(dpy,root,0,0,width,height,0,vi->depth,InputOutput,vi->visual,CWColormap | CWEventMask,&swa);
  Atom delWindow = XInternAtom(dpy,"WM_DELETE_WINDOW",0); // This is for the window close operation
  XSetWMProtocols(dpy,win,&delWindow,1);
  XMapWindow(dpy,win);
  XStoreName(dpy,win,title);
  glc = glXCreateContext(dpy,vi,nullptr,GL_TRUE);
  glXMakeCurrent(dpy,win,glc);
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
    eventTranslate(xev);
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
