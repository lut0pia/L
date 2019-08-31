#include <L/src/engine/Engine.h>
#include <L/src/system/Device.h>
#include <L/src/time/Timer.h>

#include <Windows.h>
#include <Xinput.h>

using namespace L;

typedef DWORD WINAPI XInputGetState_FN(DWORD dwUserIndex, XINPUT_STATE *pState);
XInputGetState_FN* DynamicXInputGetState;

class XInputDevice : public Device {
public:
  DWORD _user_index;
  XINPUT_STATE _state;
  Timer _inactive_timer;

  void set_name(const char* name) {
    _name = name;
  }
  inline void map_button(Device::Button b, DWORD xi_b) {
    set_button(b, (_state.Gamepad.wButtons & xi_b) != 0);
  }
  inline void map_axis(Device::Axis a, SHORT xi_a) {
    _axes[size_t(a)] = float(xi_a)/32767.f;
  }
  inline void map_axis(Device::Axis a, BYTE xi_a) {
    _axes[size_t(a)] = float(xi_a)/255.f;
  }
  void update() {
    static const Time one_second(0, 0, 1);
    // Only get state if active or every second if inactive
    // Otherwise we may easily waste half a millisecond every frame
    if(_active || _inactive_timer.every(one_second)) {
      _active = DynamicXInputGetState(_user_index, &_state) == ERROR_SUCCESS;
    }
    if(_active) {
      apply_deadzones();
      map_button(Device::Button::GamepadLeftStick, XINPUT_GAMEPAD_LEFT_THUMB);
      map_button(Device::Button::GamepadRightStick, XINPUT_GAMEPAD_RIGHT_THUMB);
      map_button(Device::Button::GamepadLeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER);
      map_button(Device::Button::GamepadRightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
      map_button(Device::Button::GamepadFaceBottom, XINPUT_GAMEPAD_A);
      map_button(Device::Button::GamepadFaceRight, XINPUT_GAMEPAD_B);
      map_button(Device::Button::GamepadFaceLeft, XINPUT_GAMEPAD_X);
      map_button(Device::Button::GamepadFaceTop, XINPUT_GAMEPAD_Y);
      map_axis(Axis::GamepadLeftStickX, _state.Gamepad.sThumbLX);
      map_axis(Axis::GamepadLeftStickY, _state.Gamepad.sThumbLY);
      map_axis(Axis::GamepadRightStickX, _state.Gamepad.sThumbRX);
      map_axis(Axis::GamepadRightStickY, _state.Gamepad.sThumbRY);
      map_axis(Axis::GamepadLeftTrigger, _state.Gamepad.bLeftTrigger);
      map_axis(Axis::GamepadRightTrigger, _state.Gamepad.bRightTrigger);
    }
  }
  void apply_deadzones() {
#define APPLY_DEADZONE(v, dz) if(abs<int32_t>(v)<=dz) {v = 0;}
    APPLY_DEADZONE(_state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    APPLY_DEADZONE(_state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    APPLY_DEADZONE(_state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    APPLY_DEADZONE(_state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
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
    sprintf(name, "XPad%zu", i+1);
    xinput_devices[i].set_name(name);
    xinput_devices[i]._user_index = DWORD(i);
  }
  Engine::add_parallel_update([]() {
    L_SCOPE_MARKER("XInput update");
    for(XInputDevice& xinput_device : xinput_devices) {
      xinput_device.update();
    }
  });
}
