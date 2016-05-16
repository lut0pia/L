#pragma once

#include "../math/Vector.h"
#include "../macros.h"
#include "../String.h"
#include "../image/Bitmap.h"

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
            LBUTTON, RBUTTON, MBUTTON,
            LAST
          } Button;
          enum {
            NONE, RESIZE, TEXT,
            BUTTONDOWN, BUTTONUP,
            MOUSEMOVE, MOUSEWHEEL
          } type;
          union {
            struct {
              int x,y;
              Button button;
            };
            char text[8];
          };
          Event();
      };
      static const int resizable = 0x1;
      static const int borderless = 0x2;
      static const int nocursor = 0x4;
      static const int opengl = 0x8;
    public:
      static void open(const char* title, int width = 800, int height = 600, int flags = 0);
      static void openFullscreen(const char* title, int flags = 0);
      static void close();
      static bool opened();
      static bool loop();
      static bool newEvent(Event&);
      static bool isPressed(Event::Button);

      static void swapBuffers();
      static void draw(const Bitmap&);

      // Setters & getters
      static void title(const char*);
      static void resize(int width, int height);
      static int width();
      static int height();
      static float aspect();
      static Vector2i mousePosition();
      static void mousePosition(const Vector2i&);
      static Vector2f normalizedMousePosition();
      static Vector2f normalizedToPixels(const Vector2f&);
  };
}
