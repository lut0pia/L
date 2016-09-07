#include "Scrollable.h"

using namespace L;
using namespace GUI;

Scrollable::Scrollable(Vector2i d, Ref<Base> child) : Sizable(d), modifier(0,0), child(child) {
  child->sParent(this);
  child->updateFromAbove(pos-modifier,clip);
  childDim = child->gDimensions();
}

void Scrollable::sModifier(Vector2i m) {
  //if(m.x >= 0 && m.y >= 0 && m.x <= childDim.x-dimensions.x && m.y <= childDim.y-dimensions.y){
  if(childDim.x() <= dimensions.x() && childDim.y() <= dimensions.y())
    modifier = Vector<2,int>(0,0);
  else
    modifier = Interval<2,int>(Vector<2,int>(0,0),childDim-dimensions).closestTo(m);
  child->updateFromAbove(pos-modifier,clip);
}
Vector2i Scrollable::gModifier() const {
  return modifier;
}
void Scrollable::sValue(Vector<2,float> m) {
  modifier = Vector<2,int>(max(0,childDim.x()-dimensions.x())*m.x(),max(0,childDim.y()-dimensions.y())*m.y());
  child->updateFromAbove(pos-modifier,clip);
}
Vector<2,float> Scrollable::gValue() const {
  L_ERROR("This feature hasn't been done yet.");
  //return Vector<2,float>(max(0,childDim.x-dimensions.x)/)
}

void Scrollable::dimensionsChanged(Base* child, Vector2i dimensions) {
  childDim = dimensions;
  sModifier(modifier);
}
void Scrollable::updateFromAbove(Vector2i pos,Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  child->updateFromAbove(pos-modifier,clip);
}
void Scrollable::draw(GL::Program& program) {
  child->draw(program);
}
bool Scrollable::event(const Window::Event& e) {
  /*
  bool eventUsed;
  Vector2i tmp;
  if((tmp = Input::mouseWheel(e))!=Vector<2,int>(0,0)){
      slider->sValue(slider->gValue()+Vector<2,float>(0,.1));
  }
  eventUsed = GUI::event(e);
  GUI::update();
  return eventUsed;
  */
  return child->event(e);
}

