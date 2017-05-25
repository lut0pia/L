#pragma once

#include "../containers/Array.h"
#include "../containers/Queue.h"
#include "../text/String.h"

namespace L {
  class Device {
  public:
    struct Event {
      const Device* _device;
      uint8_t _index : 7, _pressed : 1;
    };
    void* _id;
    String _name;
    uint32_t _buttons;
    float _axes[8];
  private:
    static Array<Device> _devices;
    static Queue<64, Event> _events;
    void* _systemData;
  public:
    inline Device() : _buttons(0), _axes{} {}
    inline float axis(int i) const { return _axes[i]; }
    inline bool button(int i) const { return (_buttons & (1<<i)) != 0; }

    static const Array<Device>& devices() { return _devices; }
    static const Device& device(void* id);
    static void init();
    static void update();
    static void processReport(void* id, const byte* data, size_t size);
    static bool newEvent(Event&);
  };
}
