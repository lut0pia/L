#include "Scrollable.h"

#include <algorithm>

using namespace L;
using namespace GUI;

Scrollable::Scrollable(Point2i d, Ref<Base> child) : Sizable(d), modifier(0,0), child(child) {
  child->sParent(this);
  child->updateFromAbove(pos-modifier,clip);
  childDim = child->gDimensions();
}

void Scrollable::sModifier(Point2i m) {
  //if(m.x >= 0 && m.y >= 0 && m.x <= childDim.x-dimensions.x && m.y <= childDim.y-dimensions.y){
  if(childDim.x() <= dimensions.x() && childDim.y() <= dimensions.y())
    modifier = Point<2,int>(0,0);
  else
    modifier = Interval<2,int>(Point<2,int>(0,0),childDim-dimensions).closestTo(m);
  child->updateFromAbove(pos-modifier,clip);
}
Point2i Scrollable::gModifier() const {
  return modifier;
}
void Scrollable::sValue(Point<2,float> m) {
  modifier = Point<2,int>(std::max(0,childDim.x()-dimensions.x())*m.x(),std::max(0,childDim.y()-dimensions.y())*m.y());
  child->updateFromAbove(pos-modifier,clip);
}
Point<2,float> Scrollable::gValue() const {
  throw Exception("This feature hasn't been done yet.");
  //return Point<2,float>(max(0,childDim.x-dimensions.x)/)
}

void Scrollable::dimensionsChanged(Base* child, Point2i dimensions) {
  childDim = dimensions;
  sModifier(modifier);
}
void Scrollable::updateFromAbove(Point2i pos,Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  child->updateFromAbove(pos-modifier,clip);
}
void Scrollable::draw() {
  child->draw();
}
bool Scrollable::event(const Window::Event& e) {
  /*
  bool eventUsed;
  Point2i tmp;
  if((tmp = Input::mouseWheel(e))!=Point<2,int>(0,0)){
      slider->sValue(slider->gValue()+Point<2,float>(0,.1));
  }
  eventUsed = GUI::event(e);
  GUI::update();
  return eventUsed;
  */
  return child->event(e);
}

