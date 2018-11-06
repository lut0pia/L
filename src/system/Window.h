#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>
#include "../container/Array.h"

namespace L {
  class Window {
  public:
    struct Event {
      enum class Type {
        None, Resize, Focus, Blur,
      };
      Type type;
      struct {
        int32_t x, y;
      };
    };
  protected:
    static Window* _instance;
    Array<Event> _events;
    uint32_t _width, _height, _flags, _cursor_x, _cursor_y;
    bool _opened = false;

  public:
    static const int resizable = 0x1;
    static const int borderless = 0x2;
    static const int nocursor = 0x4;

    Window();
    virtual ~Window() {}

    virtual void open(const char* title, uint32_t width = 800, uint32_t height = 600, uint32_t flags = 0) = 0;
    virtual void close() = 0;
    virtual void title(const char*) = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void create_vulkan_surface(VkInstance, VkSurfaceKHR*) = 0;
    virtual const char* extra_vulkan_extension() = 0;

    void open_fullscreen(const char* title, uint32_t flags = 0);

    inline static const Array<Event>& events() { return _instance->_events; }
    inline static void flush_events() { _instance->_events.clear(); }
    inline static uint32_t width() { return _instance->_width; }
    inline static uint32_t height() { return _instance->_height; }
    inline static uint32_t cursor_x() { return _instance->_cursor_x; }
    inline static uint32_t cursor_y() { return _instance->_cursor_y; }
    inline static bool opened() { return _instance->_opened; }
    inline static float aspect() { return float(width())/height(); }

    static Window* instance() { return _instance; }
  };
}
