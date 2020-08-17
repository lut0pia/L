#pragma once

#include "../container/Array.h"
#include "../container/Handle.h"
#include "../text/Symbol.h"

namespace L {
  class Device : public Handled<Device> {
  public:
    enum class Button : uint8_t {
#define DB(button) button,
#include "device_buttons.def"
#undef DB
      Last,
    };
    enum class Axis : uint8_t {
#define DA(axis) axis,
#include "device_axes.def"
#undef DA
      Last,
    };
    struct Event {
      const Device* device;
      Button button;
      bool pressed;
    };
  protected:
    Symbol _name;
    uint32_t _buttons[size_t(Button::Last)/32+1];
    float _axes[size_t(Axis::Last)];
    float _rumble;
    bool _active;

    void set_button(Button button, bool value);

    static void add_device(Device*);
    static void add_event(Event);
  public:
    inline Device() : Handled<Device>(this), _buttons {}, _axes {}, _rumble(0.f), _active(false) {
      add_device(this);
    }
    inline float axis(Axis axis) const { return _axes[size_t(axis)]; }
    inline bool button(Button button) const { return (_buttons[size_t(button)/32] & (1<<(size_t(button)%32))) != 0; }
    inline bool active() const { return _active; }

    static const Array<Device*>& devices();
    static bool new_event(Event&);

    static float any_axis(Axis axis);
    static bool any_button(Button button);

    static Symbol button_to_symbol(Button);
    static Button symbol_to_button(Symbol);

    static Symbol axis_to_symbol(Axis);
    static Axis symbol_to_axis(Symbol);
  };
}
