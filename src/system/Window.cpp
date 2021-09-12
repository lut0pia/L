#include "Window.h"

#include "System.h"

using namespace L;

Window* Window::_instance(nullptr);

Window::Window() {
  _instance = this;
}
void Window::open_fullscreen(const char* title, uint32_t flags) {
  open(title, _screen_width, _screen_height, borderless | flags);
}
