#include "Window.h"

#include "../hash.h"
#include "System.h"

using namespace L;

bool Window::_buttonstate[Window::Event::LAST] = {false};
Queue<512,Window::Event> Window::_events;
Vector2i Window::_mousePos;
int Window::_width,Window::_height,Window::_flags;

Window::Event::Event() {
  memset(this,0,sizeof(*this));
}

void Window::openFullscreen(const char* title,int flags) {
  Vector2i screenSize(System::screenSize());
  open(title,screenSize.x(),screenSize.y(),borderless | flags);
}

bool Window::newEvent(Event& e) {
  if(_events.empty())
    return false;
  else {
    e = _events.top();
    _events.pop();
    return true;
  }
}

Vector2f Window::normalizedMousePosition() {
  return Vector2f((2*(float)_mousePos.x()/_width)-1,-((2*(float)_mousePos.y()/_height)-1));
}
Vector2f Window::normalizedToPixels(const Vector2f& p) {
  return Vector2f(((p.x()+1)/2)*_width,((-p.y()+1)/2)*_height);
}

#define KEYS(CB) \
CB(A) CB(B) CB(C) CB(D) CB(E) CB(F) \
CB(G) CB(H) CB(I) CB(J) CB(K) CB(L) \
CB(M) CB(N) CB(O) CB(P) CB(Q) CB(R) \
CB(S) CB(T) CB(U) CB(V) CB(W) CB(X) \
CB(Y) CB(Z) CB(BACKSPACE) CB(TAB) CB(ENTER) \
CB(PAUSE) CB(CAPS) CB(ESCAPE) CB(SPACE) CB(SHIFT) \
CB(LSHIFT) CB(RSHIFT) CB(CTRL) CB(LCTRL) CB(RCTRL) \
CB(ALT) CB(LALT) CB(RALT) CB(LEFT) CB(RIGHT) \
CB(UP) CB(DOWN) CB(PAGEUP) CB(PAGEDOWN) CB(END) \
CB(HOME) CB(NUMLOCK) CB(NUM0) CB(NUM1) CB(NUM2) \
CB(NUM3) CB(NUM4) CB(NUM5) CB(NUM6) CB(NUM7) \
CB(NUM8) CB(NUM9) CB(F1) CB(F2) CB(F3) CB(F4) \
CB(F5) CB(F6) CB(F7) CB(F8) CB(F9) CB(F10) \
CB(F11) CB(F12) CB(LBUTTON) CB(RBUTTON) CB(MBUTTON)
uint32_t Window::buttonToHash(Event::Button b){
#define BTH(B) case Event::B: return FNV1A(#B);
  switch(b){
    KEYS(BTH)
  }
  return 0;
}
Window::Event::Button Window::hashToButton(uint32_t h){
#define HTB(B) case FNV1A(#B): return Event::B;
  switch(h){
    KEYS(HTB)
  }
  return (Event::Button)0;
}
Symbol Window::buttonToSymbol(Event::Button b){
#define BTS(B) if(b==Event::B) return Symbol(#B);
  KEYS(BTS)
    return Symbol("NONE");
}
Window::Event::Button Window::symbolToButton(Symbol s){
#define STB(B) if(s==Symbol(#B)) return Event::B;
  KEYS(STB)
    return (Event::Button)0;
}
