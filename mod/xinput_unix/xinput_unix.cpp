#include <L/src/engine/Engine.h>
#include <L/src/input/Device.h>
#include <L/src/text/String.h>

#include <dlfcn.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#undef major
#undef minor

using namespace L;

static Display *display = nullptr;

static int xi2_opcode = 0;

static Display* (*pXOpenDisplay)(_Xconst char*) = 0;
static int (*pXISelectEvents)(Display*,::Window,XIEventMask*,int) = 0;
static Bool (*pXQueryExtension)(Display*,_Xconst char*,int*,int*,int*) = 0;
static Status (*pXIQueryVersion)(Display*,int*,int*) = 0;
static XIDeviceInfo* (*pXIQueryDevice)(Display*,int,int*) = 0;
static void (*pXIFreeDeviceInfo)(XIDeviceInfo*) = 0;
static Bool (*pXGetEventData)(Display*,XGenericEventCookie*) = 0;
static void (*pXFreeEventData)(Display*,XGenericEventCookie*) = 0;
static int (*pXNextEvent)(Display*,XEvent*) = 0;
static int (*pXPending)(Display*) = 0;

class XInputDevice : public Device {
  static Array<XInputDevice*> xinput_devices;
  int _deviceid;
public:
  XInputDevice(const XIDeviceInfo *devinfo)
  : _deviceid(devinfo->deviceid) {
    _name = Symbol(String(devinfo->name).replace_all(" ","_"));
    _active = true;
    xinput_devices.push(this);
  }
  void event(const XEvent& xev) {
    int evtype(xev.xcookie.evtype);
    const XIRawEvent* rawev = (const XIRawEvent *)xev.xcookie.data;
    if(evtype==XI_RawButtonPress || evtype==XI_RawButtonRelease) {
      const bool pressed(evtype==XI_RawButtonPress);
      switch(rawev->detail) {
        case 1: set_button(Button::MouseLeft, pressed); break;
        case 2: set_button(Button::MouseMiddle, pressed); break;
        case 3: set_button(Button::MouseRight, pressed); break;
      }
    } else if(evtype==XI_KeyPress || evtype==XI_KeyRelease) {
      const bool pressed(evtype==XI_KeyPress);
      KeySym key_sym(XkbKeycodeToKeysym(display, rawev->detail, 0, 0));
      switch(key_sym) {
        #define MAP(xk,b) case xk: set_button(Button::b, pressed); break;
        MAP(XK_BackSpace, Backspace)
        MAP(XK_Tab, Tab)
        MAP(XK_Return, Enter)
        MAP(XK_Shift_L, LeftShift) MAP(XK_Shift_R, RightShift)
        MAP(XK_Control_L, LeftCtrl) MAP(XK_Control_R, RightCtrl)
        MAP(XK_Alt_L, LeftAlt) MAP(XK_Alt_R, RightAlt)
        MAP(XK_Pause, Pause)
        MAP(XK_Caps_Lock, Caps)
        MAP(XK_Escape, Escape)
        MAP(XK_KP_Space, Space)
        MAP(XK_Left, Left) MAP(XK_Right, Right) MAP(XK_Up, Up) MAP(XK_Down, Down)
        MAP(XK_Num_Lock, NumLock)
        MAP(XK_KP_1, Num1) MAP(XK_KP_2, Num2) MAP(XK_KP_3, Num3)
        MAP(XK_KP_4, Num4) MAP(XK_KP_5, Num5) MAP(XK_KP_6, Num6)
        MAP(XK_KP_7, Num7) MAP(XK_KP_8, Num8) MAP(XK_KP_9, Num9)
        MAP(XK_KP_0, Num0)
        MAP(XK_F1, F1) MAP(XK_F2, F2) MAP(XK_F3, F3) MAP(XK_F4, F4)
        MAP(XK_F5, F5) MAP(XK_F6, F6) MAP(XK_F7, F7) MAP(XK_F8, F8)
        MAP(XK_F9, F9) MAP(XK_F10, F10) MAP(XK_F11, F11) MAP(XK_F12, F12)
        #undef MAP
        default:
          if(key_sym>=XK_a && key_sym<=XK_z) {
            set_button(Button((key_sym-XK_a)+uint32_t(Button::A)), pressed);
          } else if(key_sym>=XK_A && key_sym<=XK_Z) {
            set_button(Button((key_sym-XK_Z)+uint32_t(Button::A)), pressed);
          }
          break;
      }
    } else if(evtype==XI_RawMotion) {
      _axes[uintptr_t(Axis::MouseDX)] = rawev->raw_values[0];
      _axes[uintptr_t(Axis::MouseDY)] = rawev->raw_values[1];
    }
  }
  static XInputDevice* find(int deviceid) {
    for(XInputDevice* device : xinput_devices) {
      if(device->_deviceid == deviceid) {
        return device;
      }
    }
    return nullptr;
  }
  static bool valid(const XIDeviceInfo *devinfo) {
    return (devinfo->use == XISlaveKeyboard || devinfo->use == XISlavePointer || devinfo->use == XIFloatingSlave);
  }
  static void update() {
    // Reset axes to avoid zombie values
    for(XInputDevice* device : xinput_devices) {
      device->_axes[uintptr_t(Axis::MouseDX)] = 0.f;
      device->_axes[uintptr_t(Axis::MouseDY)] = 0.f;
    }

    // Pump events
    XEvent xev;
    while(pXPending(display)) {
      pXNextEvent(display, &xev);
      if(xev.xcookie.type == GenericEvent &&
      xev.xcookie.extension == xi2_opcode &&
      pXGetEventData(display, &xev.xcookie)) {
        const XIRawEvent* rawev((const XIRawEvent*)xev.xcookie.data);
        switch (xev.xcookie.evtype) {
          case XI_RawMotion:
          case XI_RawButtonPress:
          case XI_RawButtonRelease:
          case XI_KeyPress:
          case XI_KeyRelease:
            if(XInputDevice* device = XInputDevice::find(rawev->deviceid)) {
              device->event(xev);
            }
        }
      }
      pXFreeEventData(display, &xev.xcookie);
    }
  }
};

Array<XInputDevice*> XInputDevice::xinput_devices;

static bool try_init() {
  { // Fetch functions in shared libs
    void* dll(nullptr);

    #define LOOKUP(x) { if (!(*((void**)&p##x) = dlsym(dll,#x))) return false; }
    dll = dlopen("libX11.so.6", RTLD_GLOBAL | RTLD_LAZY);
    if(dll == nullptr) {
      return false;
    }

    LOOKUP(XOpenDisplay);
    LOOKUP(XGetEventData);
    LOOKUP(XFreeEventData);
    LOOKUP(XQueryExtension);
    LOOKUP(XNextEvent);
    LOOKUP(XPending);

    dll = dlopen("libXi.so.6", RTLD_GLOBAL | RTLD_LAZY);
    if(dll == nullptr) {
      return false;
    }

    LOOKUP(XISelectEvents);
    LOOKUP(XIQueryVersion);
    LOOKUP(XIQueryDevice);
    LOOKUP(XIFreeDeviceInfo);

    #undef LOOKUP
  }

  display = pXOpenDisplay(nullptr);
  if(display == nullptr) {
    return false;
  }

  { // Ask for the XInput2 extension
    const char *ext("XInputExtension");
    int event(0), error(0), major(2), minor(0);
    if(pXQueryExtension(display, ext, &xi2_opcode, &event, &error) &&
      pXIQueryVersion(display, &major, &minor) != BadRequest) {
      return true;
    } else {
      return false;
    }
  }
}
void xinput_unix_module_init() {
  if(!try_init()) {
    warning("Couldn't load XInput2 extension, mouse/keyboard support may be harmed");
    return;
  }

  { // Select events to receive
    XIEventMask evmask;
    unsigned char mask[3] = { 0, 0, 0 };

    XISetMask(mask, XI_RawMotion);
    XISetMask(mask, XI_KeyPress);
    XISetMask(mask, XI_KeyRelease);
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);

    evmask.deviceid = XIAllDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;

    pXISelectEvents(display, DefaultRootWindow(display), &evmask, 1);
  }

  { // Create devices
    // TODO: handle hot plugging devices
    int device_count(0);
    XIDeviceInfo *device_list(pXIQueryDevice(display, XIAllDevices, &device_count));
    for (uintptr_t i(0); i < uintptr_t(device_count); i++) {
        if(XInputDevice::valid(device_list+i)) {
          Memory::new_type<XInputDevice>(device_list+i);
        }
    }
    pXIFreeDeviceInfo(device_list);
  }

  Engine::add_update(XInputDevice::update);
}
