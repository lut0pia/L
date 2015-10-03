#include "ActionListener.h"

using namespace L;
using namespace GUI;

Event::Event(Type type, int x, int y) : type(type), x(x), y(y) {}
ActionListener::ActionListener(const Ref<Base>& inner, const Function& f, const Var& param)
  : Layer(inner), _f(f), _param(param), _mouseIsOver(false) {}

bool ActionListener::event(const Window::Event& e) {
  if(e.type == Window::Event::MOUSEMOVE) {
    if(gClip().contains(Point<2,int>(e.x,e.y))) {
      if(!_mouseIsOver) {
        _mouseIsOver = true;
        _f(this,_param,Event::mouseOver);
      }
    } else if(_mouseIsOver) {
      _mouseIsOver = false;
      _f(this,_param,Event::mouseOut);
    }
  }
  if(_mouseIsOver && e.type == Window::Event::BUTTONDOWN && e.button == Window::Event::LBUTTON && _f(this,_param,Event(Event::leftClick,e.x,e.y)))
    return true;
  if(_mouseIsOver && e.type == Window::Event::MOUSEWHEEL) {
    if(_f(this,_param,Event(Event::wheel,e.x,e.y)))
      return true;
  }
  if(inner->event(e)) {
    _f(this,_param,Event::usedEvent);
    return true;
  } else return false;
}
