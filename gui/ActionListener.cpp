#include "ActionListener.h"

using namespace L;
using namespace GUI;

Event::Event(Type type, int x, int y) : type(type), x(x), y(y){}
ActionListener::ActionListener(const Ref<Base>& inner, Function f, Dynamic::Var param)
: Layer(inner), f(f), param(param), mouseIsOver(false){}

bool ActionListener::event(const Window::Event& e){
    if(e.type == Window::Event::MOUSEMOVE){
        if(gClip().contains(Point<2,int>(e.x,e.y))){
            if(!mouseIsOver){
                mouseIsOver = true;
                f(this,param,Event::mouseOver);
            }
        }
        else if(mouseIsOver){
            mouseIsOver = false;
            f(this,param,Event::mouseOut);
        }
    }
    if(mouseIsOver && e.type == Window::Event::LBUTTONDOWN && f(this,param,Event(Event::leftClick,e.x,e.y)))
        return true;
    if(mouseIsOver && e.type == Window::Event::MOUSEWHEEL){
        if(f(this,param,Event(Event::wheel,e.x,e.y)))
            return true;
    }
    if(inner->event(e)){
        f(this,param,Event::usedEvent);
        return true;
    }
    else return false;
}
