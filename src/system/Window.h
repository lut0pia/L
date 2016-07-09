#pragma once

#include "../containers/StaticRing.h"
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
        A = 'A',B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        BACKSPACE = 0xFF,TAB,ENTER,PAUSE,CAPS,ESCAPE,SPACE,
        SHIFT,LSHIFT,RSHIFT,
        CTRL,LCTRL,RCTRL,
        ALT,LALT,RALT,
        LEFT,RIGHT,UP,DOWN,
        PAGEUP,PAGEDOWN,END,HOME,NUMLOCK,
        NUM0,NUM1,NUM2,NUM3,NUM4,NUM5,NUM6,NUM7,NUM8,NUM9,
        F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        LBUTTON,RBUTTON,MBUTTON,
        LAST
      } Button;
      enum {
        NONE,RESIZE,TEXT,
        BUTTONDOWN,BUTTONUP,
        MOUSEMOVE,MOUSEWHEEL
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
  private:
    static bool _buttonstate[Event::LAST];
    static StaticRing<512,Event> _events;
    static Vector2i _mousePos;
    static int _width,_height,_flags;

  public:
    static const int resizable = 0x1;
    static const int borderless = 0x2;
    static const int nocursor = 0x4;

    static void open(const char* title,int width = 800,int height = 600,int flags = 0);
    static void openFullscreen(const char* title,int flags = 0);
    static void close();
    static bool opened();
    static bool loop();
    static bool newEvent(Event&);

    static void swapBuffers();
    static void draw(const Bitmap&);

    // Setters & getters
    static void title(const char*);
    static void resize(int width,int height);
    static inline int width(){ return _width; }
    static inline int height(){ return _height; }
    static inline float aspect(){ return (float)_width/_height; }
    static inline Vector2i mousePosition(){ return _mousePos; }
    static void mousePosition(const Vector2i&);
    static inline bool isPressed(Event::Button button){ return _buttonstate[button]; }
    static Vector2f normalizedMousePosition();
    static Vector2f normalizedToPixels(const Vector2f&);
  };
}
