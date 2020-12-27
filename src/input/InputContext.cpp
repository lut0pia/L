#include "InputContext.h"

#include "../component/Camera.h"
#include "../dev/profiling.h"

using namespace L;

Array<Handle<InputContext>> InputContext::_contexts;

InputContext::InputContext() : Handled<InputContext>(this), _buttons(0), _axes {0.f}, _previous_buttons(0), _previous_axes {0.f} {
  _name = ntos(handle().index());
  _contexts.push(handle());
}

void InputContext::set_name(const String& name) {
  _name = name;
}
void InputContext::set_input_map(const Ref<InputMap>& input_map) {
  _input_map = input_map;
}
void InputContext::set_block_mode(InputBlockMode block_mode) {
  _block_mode = block_mode;
}

bool InputContext::get_raw_button(Device::Button b) const {
  return _buttons[uintptr_t(b)];
}
bool InputContext::get_raw_button_pressed(Device::Button b) const {
  return _buttons[uintptr_t(b)] && !_previous_buttons[uintptr_t(b)];
}
bool InputContext::get_raw_button_released(Device::Button b) const {
  return !_buttons[uintptr_t(b)] && _previous_buttons[uintptr_t(b)];
}
float InputContext::get_raw_axis(Device::Axis a) const {
  return _axes[uintptr_t(a)];
}

bool InputContext::get_button(const Symbol& name) const {
  return get_axis(name) > 0.f;
}
bool InputContext::get_button_pressed(const Symbol& name) const {
  if(float* value = _previous_inputs.find(name)) {
    return get_axis(name) > 0.f && *value <= 0.f;
  }
  return false;
}
bool InputContext::get_button_released(const Symbol& name) const {
  if(float* value = _previous_inputs.find(name)) {
    return get_axis(name) <= 0.f && *value > 0.f;
  }
  return false;
}
float InputContext::get_axis(const Symbol& name) const {
  if(float* value = _inputs.find(name)) {
    return *value;
  }
  return 0.f;
}

void InputContext::update() {
  InputMask global_mask;
  global_mask.clear(0xff);
  Table<Device*, InputMask> masks;
  for(Device* device : Device::devices()) {
    masks[device].clear(0xff);
  }

  for(Handle<InputContext> context_handle : _contexts) {
    if(InputContext* context = context_handle) {
      // Save previous input
      context->_previous_buttons = context->_buttons;
      memcpy(context->_previous_axes, context->_axes, sizeof(context->_axes));
      context->_previous_inputs = context->_inputs;

      // Reset all input
      context->_buttons.clear();
      memset(context->_axes, 0, sizeof(context->_axes));
      for(auto& input : context->_inputs) {
        input.value() = 0.f;
      }

      const InputMask& read_mask = context->_device ? masks[context->_device] : global_mask;

      // Read raw input
      Device* linked_device = context->_device;
      for(const Device* device : Device::devices()) {
        if(linked_device && device != linked_device) {
          continue;
        }

        for(uintptr_t i = 0; i < uintptr_t(Device::Button::Last); i++) {
          if(read_mask[i] && device->button(Device::Button(i))) {
            context->_buttons.set(i);
          }
        }
        for(uintptr_t i = 0; i < uintptr_t(Device::Axis::Last); i++) {
          if(read_mask[i + uintptr_t(Device::Button::Last)]) {
            context->_axes[i] += device->axis(Device::Axis(i));
          }
        }
      }

      InputMask write_mask;
      write_mask.clear(0xff);
      if(InputMap* input_map = context->_input_map) {
        for(const auto& entry : *input_map) {
          const uintptr_t input_index = entry.is_button ? uintptr_t(entry.button) : uintptr_t(Device::Button::Last) + uintptr_t(entry.axis);
          const bool available = read_mask[input_index];
          if(available) {
            float value = entry.is_button
              ? float(context->_buttons[uintptr_t(entry.button)])
              : context->_axes[uintptr_t(entry.axis)];
            value += entry.offset;
            value *= entry.multiplier;
            context->_inputs[entry.name] += value;
          }
          write_mask.unset(input_index);
        }
      }

      // Apply mask to one or all devices
      switch(context->_block_mode) {
        case InputBlockMode::None: break;
        case InputBlockMode::Used:
          if(Device* device = context->_device) {
            masks[device] |= write_mask;
          } else {
            global_mask &= write_mask;
            for(auto& device_mask : masks) {
              device_mask.value() &= write_mask;
            }
          }
          break;
        case InputBlockMode::All:
          if(Device* device = context->_device) {
            masks[device].clear(0);
          } else {
            global_mask.clear(0);
            for(auto& device_mask : masks) {
              device_mask.value().clear(0);
            }
          }
          break;
      }
    }
  }
}

static Symbol none_symbol = "None", all_symbol = "All";
static InputBlockMode symbol_to_block_mode(const Symbol& sym) {
  if(sym == none_symbol) {
    return InputBlockMode::None;
  } else if(sym == all_symbol) {
    return InputBlockMode::All;
  } else {
    return InputBlockMode::Used;
  }
}
void InputContext::script_registration() {
  ScriptGlobal("input_map") = (ScriptNativeFunction)[](ScriptContext& c) {
    Ref<InputMap> input_map_ref = ref<InputMap>();
    InputMap& input_map = *input_map_ref;

    if(Ref<Array<Var>>* array = c.param(0).try_as<Ref<Array<Var>>>()) {
      for(const Var& entry_var : **array) {
        if(const Ref<Table<Var, Var>>* entry_table = entry_var.try_as<Ref<Table<Var, Var>>>()) {
          InputMapEntry entry;
          entry.name = (*entry_table)->get(Symbol("name"), Symbol());
          entry.offset = (*entry_table)->get(Symbol("offset"), 0.f);
          entry.multiplier = (*entry_table)->get(Symbol("multiplier"), 1.f);
          if(Var* button = (*entry_table)->find(Symbol("button"))) {
            entry.button = Device::symbol_to_button(*button);
            entry.is_button = true;
          } else if(Var* axis = (*entry_table)->find(Symbol("axis"))) {
            entry.axis = Device::symbol_to_axis(*axis);
            entry.is_button = false;
          }
          input_map.push(entry);
        }
      }
    }

    c.return_value() = input_map_ref;
  };
  L_SCRIPT_METHOD(InputContext, "set_name", 1, set_name(c.param(0)));
  L_SCRIPT_METHOD(InputContext, "set_input_map", 1, set_input_map(c.param(0)));
  L_SCRIPT_METHOD(InputContext, "set_block_mode", 1, set_block_mode(symbol_to_block_mode(c.param(0))));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_raw_button", 1, get_raw_button(Device::symbol_to_button(c.param(0))));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_raw_button_pressed", 1, get_raw_button_pressed(Device::symbol_to_button(c.param(0))));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_raw_button_released", 1, get_raw_button_released(Device::symbol_to_button(c.param(0))));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_raw_axis", 1, get_raw_axis(Device::symbol_to_axis(c.param(0))));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_button", 1, get_button(c.param(0)));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_button_pressed", 1, get_button_pressed(c.param(0)));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_button_released", 1, get_button_released(c.param(0)));
  L_SCRIPT_RETURN_METHOD(InputContext, "get_axis", 1, get_axis(c.param(0)));
}
