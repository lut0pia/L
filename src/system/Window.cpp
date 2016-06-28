#include "Window.h"

#include "System.h"

using namespace L;

bool Window::_buttonstate[Window::Event::LAST] = {false};
StaticRing<512,Window::Event> Window::_events;
Vector2i Window::_mousePos;
int Window::_width,Window::_height,Window::_flags;

Window::Event::Event() {
  memset(this,0,sizeof(*this));
}

void Window::openFullscreen(const char* title,int flags) {
  Vector2i screenSize(System::screenSize());
  open(title,screenSize.x(),screenSize.y(),borderless | flags);
}

bool Window::newEvent(Event& e) {
  if(_events.empty())
    return false;
  else {
    e = _events.top();
    _events.pop();
    return true;
  }
}

Vector2f Window::normalizedMousePosition() {
  return Vector2f((2*(float)_mousePos.x()/_width)-1,-((2*(float)_mousePos.y()/_height)-1));
}
Vector2f Window::normalizedToPixels(const Vector2f& p) {
  return Vector2f(((p.x()+1)/2)*_width,((-p.y()+1)/2)*_height);
}
