#include "Base.h"

using namespace L;
using namespace L::GUI;

Base::Base() : parent(NULL) {}

void Base::sParent(Base* p) {
  parent = p;
}
Base* Base::gParent() const {
  return parent;
}
void Base::dimensionsChanged(Point2i newDim) {
  if(parent) parent->dimensionsChanged(this,newDim);
}

void Base::updateAll() {
  if(parent)  parent->updateAll(); // Go back to root
  else        updateFromAbove(Point<2,int>(0,0),Interval<2,int>(Point<2,int>(0,0),gDimensions()));
}
void Base::update() {
  updateFromAbove(gPos(),(parent) ? parent->gClip() : gClip());
}
void Base::dimensionsChanged(Base* child, Point2i dimensions) {
  throw Exception("GUI: Non-container is a parent.");
}

void Base::draw(GL::Program& program) {}
bool Base::event(const Window::Event& e) {
  return false;
}
