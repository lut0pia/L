#include "Window.h"

#include "../container/Queue.h"
#include "System.h"

using namespace L;

namespace L {
  Queue<64, Window::Event> window_events;
  int window_width, window_height, window_flags, mouse_x, mouse_y;
}

void Window::open_fullscreen(const char* title, uint32_t flags) {
  const Vector2i screenSize(System::screenSize());
  open(title, screenSize.x(), screenSize.y(), borderless | flags);
}

bool Window::new_event(Event& e) {
  if(window_events.empty()) {
    return false;
  } else {
    e = window_events.top();
    window_events.pop();
    if(e.type == Event::Resize) {
      window_width = e.x;
      window_height = e.y;
    }
    return true;
  }
}

uint32_t Window::width() { return window_width; }
uint32_t Window::height() { return window_height; }