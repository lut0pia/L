# Input

## Devices
Devices can be keyboards, mice, trackpads or gamepads.
* `get_axis` is a device method that takes a symbol as argument and returns the value of that axis as float
* `get_button` is a device method that takes a symbol as argument and returns the value of that button as bool

## Events
Device events signal the press or release of a button on a specific device. They are forwarded to scripts through the `event` callback, which receives objects with `device`, `button` and `pressed` fields.

## Globals
* `get_devices` returns an object with all devices as keys
* `button_pressed` takes a symbol parameter and returns true if the button is currently pressed (by any device).

The list of buttons can be found in `src\system\device_buttons.def`.

The list of axes can be found in `src\system\device_axes.def`.
