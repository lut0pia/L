#include <L/src/container/Queue.h>
#include <L/src/engine/Engine.h>
#include <L/src/system/Device.h>

#include <Xinput.h>

using namespace L;

typedef DWORD WINAPI XInputGetState_FN(DWORD dwUserIndex, XINPUT_STATE *pState);
XInputGetState_FN* DynamicXInputGetState;

class XInputDevice : public Device {
public:
  DWORD user_index;
  XINPUT_STATE state;

  void set_name(const char* name) {
    _name = name;
  }
  inline void map_button(Device::Button b, DWORD xi_b) {
    set_button(b, (state.Gamepad.wButtons & xi_b) != 0);
  }
  void update() {
    if(DynamicXInputGetState(user_index, &state) == ERROR_SUCCESS) {
      _active = true;
      apply_deadzones();
      map_button(Device::Button::GamepadLeftStick, XINPUT_GAMEPAD_LEFT_THUMB);
      map_button(Device::Button::GamepadRightStick, XINPUT_GAMEPAD_RIGHT_THUMB);
      map_button(Device::Button::GamepadLeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER);
      map_button(Device::Button::GamepadRightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
      map_button(Device::Button::GamepadFaceBottom, XINPUT_GAMEPAD_A);
      map_button(Device::Button::GamepadFaceRight, XINPUT_GAMEPAD_B);
      map_button(Device::Button::GamepadFaceLeft, XINPUT_GAMEPAD_X);
      map_button(Device::Button::GamepadFaceTop, XINPUT_GAMEPAD_Y);
      _axes[size_t(Device::Axis::GamepadLeftStickX)] = float(state.Gamepad.sThumbLX)/32767.f;
      _axes[size_t(Device::Axis::GamepadLeftStickY)] = float(state.Gamepad.sThumbLY)/32767.f;
      _axes[size_t(Device::Axis::GamepadRightStickX)] = float(state.Gamepad.sThumbRX)/32767.f;
      _axes[size_t(Device::Axis::GamepadRightStickY)] = float(state.Gamepad.sThumbRY)/32767.f;
    } else {
      _active = false;
    }
  }
  void apply_deadzones() {
#define APPLY_DEADZONE(v, dz) if(abs<int32_t>(v)<=dz) {v = 0;}
    APPLY_DEADZONE(state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    APPLY_DEADZONE(state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    APPLY_DEADZONE(state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    APPLY_DEADZONE(state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
#undef APPLY_DEADZONE
  }
};

static XInputDevice xinput_devices[XUSER_MAX_COUNT];

void xinput_win_module_init() {
  HMODULE xinput_lib(LoadLibrary("xinput1_4.dll"));
  if(!xinput_lib) {
    xinput_lib = LoadLibrary("xinput1_3.dll");
  }
  if(!xinput_lib) {
    warning("Couldn't load XInput, controller support may be harmed");
    return;
  }

  DynamicXInputGetState = (XInputGetState_FN*)GetProcAddress(xinput_lib, "XInputGetState");

  if(!DynamicXInputGetState) {
    warning("Couldn't load XInput function, controller support may be harmed");
    return;
  }

  for(uintptr_t i(0); i<XUSER_MAX_COUNT; i++) {
    char name[256];
    sprintf(name, "XPad%d", i+1);
    xinput_devices[i].set_name(name);
    xinput_devices[i].user_index = DWORD(i);
  }
  Engine::add_update([]() {
    for(XInputDevice& xinput_device : xinput_devices) {
      xinput_device.update();
    }
  });
}
