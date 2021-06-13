#include "Window.h"

#include "System.h"

using namespace L;

Window* Window::_instance(nullptr);

Window::Window() {
  _instance = this;
}
void Window::open_fullscreen(const char* title, uint32_t flags) {
  const Vector2i screen_size = System::screen_size();
  open(title, screen_size.x(), screen_size.y(), borderless | flags);
}
