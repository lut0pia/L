#include <L/src/container/Array.h>
#include <L/src/engine/Engine.h>
#include <L/src/system/Device.h>
#include <L/src/system/File.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

using namespace L;

// Reference: https://github.com/drewnoakes/joystick/blob/master/joystick-api.txt
// If unavailable, search joystick-api.txt in your favorite search engine

struct js_event {
	uint32_t time;
	int16_t value;
	uint8_t type;
	uint8_t number;
};

#define JS_EVENT_BUTTON 0x01
#define JS_EVENT_AXIS   0x02
#define JS_EVENT_INIT   0x80

class JSDevice : public Device {
protected:
  int _fd;
  js_event _jse;
public:
  JSDevice(int fd) : _fd(fd) {}
  void map_button(Device::Button b, uint8_t js_b) {
    if(_jse.number==js_b) {
      set_button(b, _jse.value);
    }
  }
  void map_axis(Device::Axis a, uint8_t js_a, bool invert = false, bool clamp = false) {
    if(_jse.number==js_a) {
      float value(_jse.value/32768.f);
      if(abs(value)<0.2f) value = 0.f;
      if(invert) value = -value;
      if(clamp) value = L::clamp(value, 0.f, 1.f);
      _axes[uintptr_t(a)] = value;
    }
  }
  void update() {
    int bytes_read;
    while((bytes_read = read(_fd, &_jse, sizeof(_jse))) == sizeof(_jse)) {
      bool issue_event(true);
      if(_jse.type&JS_EVENT_INIT) { // It's an init event
        issue_event = false; // We don't want to create events for that
        _jse.type &= ~JS_EVENT_INIT;
      }
      switch(_jse.type) {
        case JS_EVENT_BUTTON:
          map_button(Button::GamepadFaceBottom, 0);
          map_button(Button::GamepadFaceRight, 1);
          map_button(Button::GamepadFaceLeft, 2);
          map_button(Button::GamepadFaceTop, 3);
          map_button(Button::GamepadLeftShoulder, 4);
          map_button(Button::GamepadRightShoulder, 5);
          map_button(Button::GamepadLeftStick, 9);
          map_button(Button::GamepadRightStick, 10);
          break;
        case JS_EVENT_AXIS:
          map_axis(Axis::GamepadLeftStickX, 0);
          map_axis(Axis::GamepadLeftStickY, 1, true);
          map_axis(Axis::GamepadRightStickX, 3);
          map_axis(Axis::GamepadRightStickY, 4, true);
          map_axis(Axis::GamepadLeftTrigger, 2, false, true);
          map_axis(Axis::GamepadRightTrigger, 5, false, true);
          break;
      }
    }
  }
};

static Array<JSDevice*> js_devices;
void joystick_unix_module_init() {
  Array<String> joysticks(File::list("/dev/input/js*"));
  for(const String& joystick : joysticks) {
    int fd(open(joystick, O_RDONLY | O_NONBLOCK));
    if(fd>=0) {
      JSDevice* device(Memory::new_type<JSDevice>(fd));
      js_devices.push(device);
    }
  }
  Engine::add_update([](){
    for(JSDevice* device : js_devices) {
      device->update();
    }
  });
}
