#include "Base.h"

using namespace L;
using namespace L::GUI;

Base::Base() : parent(nullptr) {}

void Base::sParent(Base* p) {
  parent = p;
}
Base* Base::gParent() const {
  return parent;
}
void Base::dimensionsChanged(Vector2i newDim) {
  if(parent) parent->dimensionsChanged(this,newDim);
}

void Base::updateAll() {
  if(parent)  parent->updateAll(); // Go back to root
  else        updateFromAbove(Vector<2,int>(0,0),Interval<2,int>(Vector<2,int>(0,0),gDimensions()));
}
void Base::update() {
  updateFromAbove(gPos(),(parent) ? parent->gClip() : gClip());
}
void Base::dimensionsChanged(Base* child, Vector2i dimensions) {
  throw Exception("GUI: Non-container is a parent.");
}

void Base::draw(GL::Program& program) {}
bool Base::event(const Window::Event& e) {
  return false;
}
