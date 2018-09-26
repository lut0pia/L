#pragma once

#include <stdint.h>

namespace L {
  class Window {
  public:
    class Event {
    public:
      enum Type {
        None, Resize, Focus, Blur,
      };
      Type type;
      struct {
        int32_t x, y;
      };
      constexpr Event() : type(None), x(0), y(0) {}
    };
  public:
    static const int resizable = 0x1;
    static const int borderless = 0x2;
    static const int nocursor = 0x4;

    static void open(const char* title, uint32_t width = 800, uint32_t height = 600, uint32_t flags = 0);
    static void open_fullscreen(const char* title, uint32_t flags = 0);
    static void close();
    static bool opened();
    static bool loop();
    static bool new_event(Event&);

    static void title(const char*);
    static void resize(uint32_t width, uint32_t height);
    static uint32_t width();
    static uint32_t height();
    static uint32_t mouse_x();
    static uint32_t mouse_y();
    static inline float aspect() { return float(width())/height(); }
  };
}
