#include "Slider.h"

using namespace L;
using namespace GUI;

Slider::Slider(Vector2i d, Ref<Base> thumb) : Sizable(d), curDist(0,0) {
  sThumb(thumb);
  inMotion = false;
}
void Slider::sThumb(Ref<Base> thumb) {
  this->thumb = thumb;
  thumb->sParent(this);
  sValue(Vector<2,float>(0,0));
}
void Slider::sValue(Vector<2,float> v) {
  /*
  Vector2i thumbDim(thumb->gDimensions());
  v.x *= (float)(dimensions.x()-thumbDim.x());
  v.y *= (float)(dimensions.y()-thumbDim.y());
  */
  thumbPos = (dimensions - thumb->gDimensions()) * v;
  updateThumb();
}

Vector<2,float> Slider::gValue() const {
  return value;
}
void Slider::updateThumb() {
  thumb->updateFromAbove(pos+thumbPos,clip);
}
void Slider::updateValue() {
  Vector2i thumbDim(thumb->gDimensions());
  value = Vector<2,float>((float)thumbPos.x()/(float)(dimensions.x()-thumbDim.x()),
                         (float)thumbPos.y()/(float)(dimensions.x()-thumbDim.y()));
  updateThumb();
}
void Slider::dimensionsChanged(Base* e,Vector2i thumbDim) {
  Vector<2,float> v(value);
  v.x() *= (float)(dimensions.x()-thumbDim.x());
  v.y() *= (float)(dimensions.y()-thumbDim.y());
  thumbPos = v;
  updateThumb();
}
void Slider::updateFromAbove(Vector2i pos,Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  updateThumb();
}
void Slider::draw(GL::Program& program) {
  thumb->draw(program);
}
bool Slider::event(const Window::Event& e) {
  bool wtr = false;
  if(e.type == Window::Event::BUTTONDOWN && e.button == Window::Event::LBUTTON && thumb->gClip().contains(Vector<2,int>(e.x,e.y))) {
    inMotion = true;
    curDist = Vector<2,int>(e.x,e.y) - thumb->gPos();
    wtr = true;
  }
  if(e.type == Window::Event::BUTTONUP && e.button == Window::Event::LBUTTON)
    inMotion = false;
  if(inMotion) {
    thumbPos = (Interval<2,int>(Vector<2,int>(0,0),dimensions-thumb->gDimensions()) // Interval of inside values
                .closestTo(Vector<2,int>(e.x,e.y)-pos-curDist));
    updateValue();
    wtr = true;
  }
  return wtr;
}

