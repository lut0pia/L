#include "Layer.h"

using namespace L;
using namespace GUI;

Layer::Layer(const Ref<Base>& inner) : inner(inner) {
  inner->sParent(this);
}

Vector2i Layer::gPos() {
  return inner->gPos();
}
Interval2i Layer::gClip() {
  return inner->gClip();
}
Vector2i Layer::gDimensions() {
  return inner->gDimensions();
}
void Layer::dimensionsChanged(Base* e,Vector2i newDim) {
  Base::dimensionsChanged(newDim);
}
void Layer::updateFromAbove(Vector2i pos,Interval2i parentClip) {
  inner->updateFromAbove(pos,parentClip);
}
void Layer::draw(GL::Program& program) {
  inner->draw(program);
}
bool Layer::event(const Window::Event& e) {
  return inner->event(e);
}
