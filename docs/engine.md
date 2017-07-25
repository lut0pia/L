# Engine reference

This file documents how to interact with the engine in [script](script.md) in general.
For now, when the engine starts, it reads a script in the current directory named "startup.ls".

## Entities
```clojure
(local e1 (entity-make)) ; Create a new entity
(local e2 (entity-copy e1)) ; Copy an entity
(entity-destroy e1) ; Destroy an entity
```

## Physics
```clojure
(engine-gravity (vec 0 0 -9.8)) ; Sets the gravity (default is zero)
```

## Input
`button-pressed` takes a symbol parameter and returns true if the button is currently pressed.
```clojure
(if (button-pressed 'A) (print "Pressing A key!\n"))
(if (button-pressed 'LBUTTON) (print "Pressing left mouse button!\n"))
```
Here's a list of the buttons currently mapped:
A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, BACKSPACE, TAB, ENTER, PAUSE, CAPS, ESCAPE, SPACE, SHIFT, LSHIFT, RSHIFT, CTRL, LCTRL, RCTRL, ALT, LALT, RALT, LEFT, RIGHT, UP, DOWN, PAGEUP, PAGEDOWN, END, HOME, NUMLOCK, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, LBUTTON, RBUTTON, MBUTTON

This list also applies to event buttons sent to the event handler in Script components.
