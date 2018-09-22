#include "Device.h"

#include "../container/Queue.h"

using namespace L;

namespace L {
  Array<Device*> device_list;
  Queue<64, Device::Event> device_events;
}

void Device::set_button(Button b, bool value) {
  const bool old_value(button(b));
  if(value) {
    _buttons[size_t(b)/32] |= (1<<(size_t(b)%32));
  } else {
    _buttons[size_t(b)/32] &= ~(1<<(size_t(b)%32));
  }
  if(old_value != value) {
    add_event(Event {this, b, value});
  }
}

void Device::add_device(Device* d) {
  device_list.push(d);
}
void Device::add_event(Event e) {
  device_events.push(e);
}

const Array<Device*>& Device::devices() {
  return device_list;
}
bool Device::new_event(Event& e) {
  if(device_events.empty()) {
    return false;
  } else {
    e = device_events.top();
    device_events.pop();
    return true;
  }
}

float Device::any_axis(Axis axis) {
  for(const Device* device : device_list) {
    if(device->active() && device->axis(axis)!=0.f) {
      return device->axis(axis);
    }
  }
  return 0.f;
}
bool Device::any_button(Button button) {
  for(const Device* device : device_list) {
    if(device->active() && device->button(button)) {
      return true;
    }
  }
  return false;
}

Symbol Device::button_to_symbol(Button b) {
#define DB(button) if(b==Button::button) return Symbol(#button);
#include "device_buttons.def"
#undef DB
  return Symbol("None");
}
Device::Button Device::symbol_to_button(Symbol s) {
#define DB(button) if(s==Symbol(#button)) return Button::button;
#include "device_buttons.def"
#undef DB
  return Button::Last;
}

Symbol Device::axis_to_symbol(Axis a) {
#define DA(axis) if(a==Axis::axis) return Symbol(#axis);
#include "device_axes.def"
#undef DA
  return Symbol("None");
}
Device::Axis Device::symbol_to_axis(Symbol s) {
#define DA(axis) if(s==Symbol(#axis)) return Axis::axis;
#include "device_axes.def"
#undef DA
  return Axis::Last;
}
