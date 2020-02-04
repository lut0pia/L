#include <L/src/container/Queue.h>
#include <L/src/dev/profiling.h>
#include <L/src/engine/Engine.h>
#include <L/src/parallelism/TaskSystem.h>
#include <L/src/system/Device.h>

#include <L/src/stream/CFileStream.h>

#include <Windows.h>
#include <hidsdi.h>
#include <mfobjects.h>

using namespace L;

static Symbol mouse_symbol("Mouse"), keyboard_symbol("Keyboard");

class RawInputDevice : public Device {
public:
  PHIDP_PREPARSED_DATA _preparsed;
  uint32_t _min_button_usage;
  HIDP_CAPS _caps;
  HIDP_BUTTON_CAPS _button_caps[8];
  HIDP_VALUE_CAPS _value_caps[16];
  HANDLE hDevice;

  void set_active(bool active) { _active = active; }
  void frame_reset() {
    _axes[uintptr_t(Axis::MouseDX)] = 0.f;
    _axes[uintptr_t(Axis::MouseDY)] = 0.f;
  }
  void process_rawinput(const RAWINPUT* rawinput) {
    switch(rawinput->header.dwType) {
      case RIM_TYPEMOUSE:
        _name = mouse_symbol;
#define MOUSE_BUTTON(rb,b) \
if(rawinput->data.mouse.usButtonFlags & RI_MOUSE_ ## rb ## _DOWN) set_button(b,true); \
else if(rawinput->data.mouse.usButtonFlags & RI_MOUSE_ ## rb ## _UP) set_button(b,false);
        MOUSE_BUTTON(LEFT_BUTTON, Button::MouseLeft);
        MOUSE_BUTTON(MIDDLE_BUTTON, Button::MouseMiddle);
        MOUSE_BUTTON(RIGHT_BUTTON, Button::MouseRight);
#undef MOUSE_BUTTON
        _axes[uintptr_t(Axis::MouseDX)] += rawinput->data.mouse.lLastX;
        _axes[uintptr_t(Axis::MouseDY)] += rawinput->data.mouse.lLastY;
        break;

      case RIM_TYPEKEYBOARD:
        _name = keyboard_symbol;
        switch(rawinput->data.keyboard.VKey) {
#define KB(rb,b) case rb: set_button(Button::b,rawinput->data.keyboard.Message==WM_KEYDOWN); break
          KB(VK_BACK, Backspace);
          KB(VK_TAB, Tab);
          KB(VK_RETURN, Enter);
          KB(VK_LSHIFT, LeftShift); KB(VK_RSHIFT, RightShift);
          KB(VK_LCONTROL, LeftCtrl); KB(VK_RCONTROL, RightCtrl);
          KB(VK_LMENU, LeftAlt); KB(VK_RMENU, RightAlt);
          KB(VK_PAUSE, Pause);
          KB(VK_CAPITAL, Caps);
          KB(VK_ESCAPE, Escape);
          KB(VK_SPACE, Space);
          KB(VK_LEFT, Left); KB(VK_RIGHT, Right); KB(VK_UP, Up); KB(VK_DOWN, Down);
          KB(VK_NUMLOCK, NumLock);
          KB(VK_NUMPAD1, Num1); KB(VK_NUMPAD2, Num2); KB(VK_NUMPAD3, Num3);
          KB(VK_NUMPAD4, Num4); KB(VK_NUMPAD5, Num5); KB(VK_NUMPAD6, Num6);
          KB(VK_NUMPAD7, Num7); KB(VK_NUMPAD8, Num8); KB(VK_NUMPAD9, Num9);
          KB(VK_NUMPAD0, Num0);
          KB(VK_F1, F1); KB(VK_F2, F2); KB(VK_F3, F3); KB(VK_F4, F4);
          KB(VK_F5, F5); KB(VK_F6, F6); KB(VK_F7, F7); KB(VK_F8, F8);
          KB(VK_F9, F9); KB(VK_F10, F10); KB(VK_F11, F11); KB(VK_F12, F12);
          KB('A', A); KB('B', B); KB('C', C); KB('D', D);
          KB('E', E); KB('F', F); KB('G', G); KB('H', H);
          KB('I', I); KB('J', J); KB('K', K); KB('L', L);
          KB('M', M); KB('N', N); KB('O', O); KB('P', P);
          KB('Q', Q); KB('R', R); KB('S', S); KB('T', T);
          KB('U', U); KB('V', V); KB('W', W); KB('X', X);
          KB('Y', Y); KB('Z', Z);
#undef KB
        }
        break;
    }
  }
};

static Array<RawInputDevice*> rawinput_devices;
static Array<RAWINPUTDEVICELIST> rawinput_device_lists;

void rawinput_module_init() {
  RAWINPUTDEVICE Rid[4] {};

  Rid[0].usUsagePage = 0x01;
  Rid[0].usUsage = 0x01; // Mouse
  Rid[1].usUsagePage = 0x01;
  Rid[1].usUsage = 0x02; // Mouse
  Rid[2].usUsagePage = 0x01;
  Rid[2].usUsage = 0x06; // Keyboard
  Rid[3].usUsagePage = 0x01;
  Rid[3].usUsage = 0x07; // Keyboard

  if(!RegisterRawInputDevices(Rid, L_COUNT_OF(Rid), sizeof(RAWINPUTDEVICE))) {
    warning("Couldn't register raw input devices, mouse support may be harmed");
    return;
  }

  Engine::add_parallel_update([]() {
    L_SCOPE_MARKER("RawInput update");
    L_SCOPE_THREAD_MASK(1);

    { // Fetch rawinput devices
      UINT rawinput_device_count;
      GetRawInputDeviceList(nullptr, &rawinput_device_count, sizeof(RAWINPUTDEVICELIST));
      rawinput_device_lists.size(rawinput_device_count);
      GetRawInputDeviceList(rawinput_device_lists.begin(), &rawinput_device_count, sizeof(RAWINPUTDEVICELIST));
    }

    for(const RAWINPUTDEVICELIST& rawinput_device_list : rawinput_device_lists) {
      RawInputDevice* rawinput_device(nullptr);

      // Search for already created device with same handle
      for(RawInputDevice* rawinput_device_candidate : rawinput_devices) {
        if(rawinput_device_list.hDevice == rawinput_device_candidate->hDevice) {
          rawinput_device = rawinput_device_candidate;
          break;
        }
      }

      // TODO: handle device activity

      if(rawinput_device) {
        rawinput_device->set_active(true);
        break; // Already created this rawinput device
      }

      // Create new rawinput device
      rawinput_device = Memory::new_type<RawInputDevice>();
      rawinput_devices.push(rawinput_device);
      rawinput_device->hDevice = rawinput_device_list.hDevice;
      rawinput_device->set_active(true);

      { // Get preparsed data
        UINT preparsed_data_size;
        GetRawInputDeviceInfo(rawinput_device_list.hDevice, RIDI_PREPARSEDDATA, nullptr, &preparsed_data_size);

        if(!preparsed_data_size)
          continue;

        rawinput_device->_preparsed = (PHIDP_PREPARSED_DATA)Memory::alloc(preparsed_data_size);

        if(GetRawInputDeviceInfo(rawinput_device_list.hDevice, RIDI_PREPARSEDDATA, rawinput_device->_preparsed, &preparsed_data_size)==(UINT)-1)
          error("GetRawInputDeviceInfo failed");
        if(!HidP_GetCaps(rawinput_device->_preparsed, &rawinput_device->_caps))
          error("Couldn't get caps");
      }
    }

    for(RawInputDevice* rawinput_device : rawinput_devices) {
      rawinput_device->frame_reset(); // Reset mouse delta
    }

    { // Fetch events
      UINT rawinput_buffer_size;
      GetRawInputBuffer(nullptr, &rawinput_buffer_size, sizeof(RAWINPUTHEADER));

      if(!rawinput_buffer_size) {
        return;
      }

      rawinput_buffer_size *= 16; // ???
      RAWINPUT* rawinput_buffer = (RAWINPUT*)Memory::alloc(rawinput_buffer_size);

      UINT rawinput_count = GetRawInputBuffer(rawinput_buffer, &rawinput_buffer_size, sizeof(RAWINPUTHEADER));

      RAWINPUT* rawinput = rawinput_buffer;
      for(unsigned int i = 0; i < rawinput_count; ++i) {
        for(RawInputDevice* rawinput_device : rawinput_devices) {
          if(rawinput_device->hDevice == rawinput->header.hDevice) {
            rawinput_device->process_rawinput(rawinput);
          }
        }

        rawinput = NEXTRAWINPUTBLOCK(rawinput);
      }
      Memory::free(rawinput_buffer, rawinput_buffer_size);
    }
  });
}
