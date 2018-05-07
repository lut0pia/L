#include "Device.h"

#include "../macros.h"

#include <Windows.h>
#include <hidsdi.h>

using namespace L;

struct DeviceSystem {
  L_ALLOCABLE(DeviceSystem)
public:
  PHIDP_PREPARSED_DATA _preparsed;
  uint32_t _minButtonUsage;
  HIDP_CAPS _caps;
  HIDP_BUTTON_CAPS _buttonCaps[8];
  HIDP_VALUE_CAPS _valueCaps[16];
};

void Device::init() {
  RAWINPUTDEVICE Rid[2];

  Rid[0].usUsagePage = 0x01;
  Rid[0].usUsage = 0x05;
  Rid[0].dwFlags = 0;                 // adds game pad
  Rid[0].hwndTarget = 0;

  Rid[1].usUsagePage = 0x01;
  Rid[1].usUsage = 0x04;
  Rid[1].dwFlags = 0;                 // adds joystick
  Rid[1].hwndTarget = 0;

  if(!RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)))
    error("Couldn't register raw input devices");

  UINT nDevices(32);
  RAWINPUTDEVICELIST pRawInputDeviceList[32];

  if((nDevices = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)))==(UINT)-1)
    error("More than 32 HIDs connected.");

  for(UINT i(0); i<nDevices; i++) {
    DeviceSystem* deviceSystem = new DeviceSystem;

    UINT preparsedDataSize;
    GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_PREPARSEDDATA, NULL, &preparsedDataSize);

    if(!preparsedDataSize)
      continue;

    deviceSystem->_preparsed = (PHIDP_PREPARSED_DATA)Memory::alloc(preparsedDataSize);

    if(GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_PREPARSEDDATA, deviceSystem->_preparsed, &preparsedDataSize)==(UINT)-1)
      error("GetRawInputDeviceInfo failed");
    if(!HidP_GetCaps(deviceSystem->_preparsed, &deviceSystem->_caps))
      error("Couldn't get caps");
    if(deviceSystem->_caps.Usage!=5 || deviceSystem->_caps.UsagePage!=1) { // Not a gamepad
      HidD_FreePreparsedData(deviceSystem->_preparsed);
      delete deviceSystem;
      continue;
    }

    _devices.push();
    Device& device(_devices.back());
    device._id = pRawInputDeviceList[i].hDevice;
    device._systemData = deviceSystem;

    L_ASSERT(deviceSystem->_caps.NumberInputButtonCaps<sizeof(deviceSystem->_buttonCaps)/sizeof(HIDP_BUTTON_CAPS));
    if(HidP_GetButtonCaps(HidP_Input, deviceSystem->_buttonCaps, &deviceSystem->_caps.NumberInputButtonCaps, deviceSystem->_preparsed) != HIDP_STATUS_SUCCESS)
      error("Couldn't get button caps.");
    if(HidP_GetValueCaps(HidP_Input, deviceSystem->_valueCaps, &deviceSystem->_caps.NumberInputValueCaps, deviceSystem->_preparsed) != HIDP_STATUS_SUCCESS)
      error("Couldn't get value caps.");

    deviceSystem->_minButtonUsage = deviceSystem->_buttonCaps[0].Range.UsageMin;
  }
}
void Device::update() {}

void Device::processReport(void* id, const uint8_t* data, size_t size) {
  for(auto&& device : _devices) {
    if(device._id==id) {
      const DeviceSystem* deviceSystem((const DeviceSystem*)device._systemData);

      { // Handle buttons
        const USAGE buttonUsagePage(9);
        USAGE usage[32];
        ULONG usageLength(ULONG(sizeof(usage)/sizeof(USAGE)));
        if(HidP_GetUsages(HidP_Input, buttonUsagePage, 0, usage, &usageLength, deviceSystem->_preparsed, PCHAR(data), ULONG(size)) != HIDP_STATUS_SUCCESS)
          error("Could not get controller usages.");

        uint32_t newButtons(0);
        for(uintptr_t i(0); i<usageLength; i++)
          newButtons |= 1<<(usage[i]-deviceSystem->_minButtonUsage);

        for(uint8_t i(0); i<32; i++)
          if((device._buttons & (1<<i)) != (newButtons & (1<<i))) {
            _events.push(Event{&device,i,(newButtons & (1<<i)) != 0});
            //out << "Button " << i << ((newButtons & (1<<i)) ? " pressed" : " released") << '\n';
          }
        device._buttons = newButtons;
      }

      { // Handle axes
        uint8_t index(0);
        for(uint8_t i(0); i<deviceSystem->_caps.NumberInputValueCaps; i++) {
          const HIDP_VALUE_CAPS& valueCap(deviceSystem->_valueCaps[i]);
          int usageValue;
          if(HidP_GetUsageValue(HidP_Input, valueCap.UsagePage, 0, valueCap.NotRange.Usage, (ULONG*)&usageValue, deviceSystem->_preparsed, PCHAR(data), ULONG(size)) != HIDP_STATUS_SUCCESS)
            error("Could not get controller value usage.");
          float value(usageValue);
          switch(valueCap.BitSize) {
            case 4: continue;
            case 8:
              if(valueCap.LogicalMin<0) {
                usageValue = *(int8_t*)&usageValue;
                value = usageValue/128.f;
              } else
                value = usageValue/128.f-1.f;
              break;
            case 16:
              if(valueCap.LogicalMin<0) {
                usageValue = *(int16_t*)&usageValue;
                value = usageValue/32768.f;
              } else
                value = usageValue/32768.f-1.f;
              break;
          }
          device._axes[index] = (abs(value)>.16f) ? value : 0.f;
          //out << "Value " << i << " at " << usageValue << " (" << ntos<2>((uint8_t)value,8) << ")" << '\n';
          index++;
        }
      }
      return;
    }
  }
}
