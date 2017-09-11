#pragma once

#include "../container/Queue.h"
#include "../math/Vector.h"
#include "../text/String.h"
#include "../image/Bitmap.h"
#include "../text/Symbol.h"

namespace L {
  class Window {
  public:
    class Event {
    public:
      enum Button{
        // '0'-'9'
        A = 'A',B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        Backspace = 0xFF,Tab,Enter,Pause,Caps,Escape,Space,
        Shift,LeftShift,RightShift,
        Ctrl,LeftCtrl,RightCtrl,
        Alt,LeftAlt,RightAlt,
        Left,Right,Up,Down,
        PageUp,PageDown,End,Home,NumLock,
        Num0,Num1,Num2,Num3,Num4,Num5,Num6,Num7,Num8,Num9,
        F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        LeftButton,RightButton,MiddleButton,
        Last
      };
      enum Type{
        None,Resize,Text,
        ButtonDown,ButtonUp,
        MouseMove,MouseWheel
      };
      Type type;
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
    static bool _buttonstate[Event::Last];
    static Queue<512,Event> _events;
    static Vector2i _mousePos;
    static int _width,_height,_flags;

  public:
    static const int resizable = 0x1;
    static const int borderless = 0x2;
    static const int nocursor = 0x4;
    static const int loopcursor = 0x8;

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
    static uint32_t buttonToHash(Event::Button);
    static Event::Button hashToButton(uint32_t);
    static Symbol buttonToSymbol(Event::Button);
    static Event::Button symbolToButton(Symbol);
    static Symbol event_type_to_symbol(Event::Type);
  };
}
