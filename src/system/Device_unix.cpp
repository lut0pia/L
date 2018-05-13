#include "Device.h"

#include "../macros.h"
#include "../system/File.h"

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

void Device::init() {
  Array<String> joysticks(File::list("/dev/input/js*"));
  for(const String& joystick : joysticks) {
    int fd(open(joystick, O_RDONLY | O_NONBLOCK));
    if(fd>=0) {
      _devices.push();
      Device& device(_devices.back());
      device._id = (void*)fd;
    }
  }
}
void Device::update() {
  js_event jse;
  for(Device& device : _devices) {
    int fd(*(int*)&device._id);
    int bytes_read;
    while((bytes_read = read(fd, &jse, sizeof(jse))) == sizeof(jse)) {
      bool issue_event(true);
      if(jse.type&JS_EVENT_INIT) { // It's an init event
        issue_event = false; // We don't want to create events for that
        jse.type &= ~JS_EVENT_INIT;
      }
      switch(jse.type) {
        case JS_EVENT_BUTTON:
          if(jse.value) device._buttons |= 1<<jse.number;
          else device._buttons &= ~(1<<jse.number);
          if(issue_event) _events.push(Event{&device,jse.number,jse.value});
          break;
        case JS_EVENT_AXIS: {
          float value(jse.value/32768.f);
          device._axes[jse.number] = (abs(value)>.16f) ? value : 0.f;
          break;
        }
      }
    }
  }
}

void Device::processReport(void* id,const uint8_t* data,size_t size) {}
