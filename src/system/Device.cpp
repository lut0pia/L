#include "Device.h"

using namespace L;

Array<Device> Device::_devices;
StaticRing<64,Device::Event> Device::_events;

bool Device::newEvent(Event& e){
  if(_events.empty()) return false;
  e = _events.top();
  _events.pop();
  return true;
}
