#ifndef DEF_L_System_Window
#define DEF_L_System_Window

#include <iostream>
#include "../geometry/Point.h"
#include "../macros.h"
#include "../stl/String.h"

namespace L {
  class Window {
    public:
      class Event {
        public:
          typedef enum {
            // '0'-'9'
            A = 'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            BACKSPACE = 0xFF, TAB, ENTER, PAUSE, CAPS, ESCAPE, SPACE,
            SHIFT, LSHIFT, RSHIFT,
            CTRL, LCTRL, RCTRL,
            ALT, LALT, RALT,
            LEFT, RIGHT, UP, DOWN,
            PAGEUP, PAGEDOWN, END, HOME, NUMLOCK,
            NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
            LAST
          } VKey;
          enum {
            NONE, RESIZE, TEXT,
            KEYDOWN, KEYUP,
            MOUSEMOVE, MOUSEWHEEL,
            LBUTTONDOWN, LBUTTONUP, RBUTTONDOWN, RBUTTONUP
          } type;
          union {
            struct {int x,y;};
            VKey key;
            char text[8];
          };
          Event();
      };
      static const size_t resizable = 1;
    public:
      static void open(const String& title, size_t width = 800, size_t height = 600, size_t flags = 0);
      static void close();
      static bool opened();
      static bool loop();
      static bool newEvent(Event&);
      static bool isPressed(Event::VKey);
      static void swapBuffers();

      // Setters & getters
      static void title(const String&);
      static void resize(int width, int height);
      static Point2i mousePosition();
      static Point2f normalizedMousePosition();
  };
}

#endif





