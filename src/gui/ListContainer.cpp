#include "ListContainer.h"

#include <algorithm>
#include "GUI.h"

using namespace L;
using namespace L::GUI;

ListContainer::ListContainer() : Solid(), spacing(0), blockInsert(false) {}
ListContainer::ListContainer(int spacing) : Solid(), spacing(spacing), blockInsert(false) {}

void ListContainer::toggleBlockInsert() {
  blockInsert = !blockInsert;
  update();
}
void ListContainer::push_back(Ref<Base> e) {
  elements.push(e);
  e->sParent(this);
  update();
}
void ListContainer::pop_back(size_t n) {
  while(n--)
    elements.pop();
  update();
}
size_t ListContainer::size() {
  return elements.size();
}
void ListContainer::clear() {
  elements.clear();
  update();
}

Vector2i ListContainer::gDimensions() {
  return dimensions;
}
void ListContainer::dimensionsChanged(Base* e, Vector2i newDim) {
  update();
}
void ListContainer::updateFromAbove(Vector2i pos, Interval2i parentClip) {
  this->pos = pos;
  clip = parentClip;
  if(!blockInsert) {
    Vector2i oldDim(dimensions);
    dimensions.x() = dimensions.y() = 0;
    for(uint i(0); i<elements.size(); i++) {
      const Ref<Base>& element(elements[i]);
      element->updateFromAbove(Vector<2,int>(pos.x(),pos.y()+dimensions.y()),parentClip);
      Vector2i childDim(element->gDimensions());
      dimensions.y() += childDim.y();
      if(i+1<elements.size())
        dimensions.y() += spacing;
      dimensions.x() = std::max(dimensions.x(), childDim.x());
    }
    if(dimensions != oldDim)
      Base::dimensionsChanged(dimensions);
  }
}
void ListContainer::draw(GL::Program& program) {
  for(auto&& element : elements)
    element->draw(program);
}
bool ListContainer::event(const Window::Event& e) {
  for(auto&& element : elements)
    if(element->event(e))
      return true;
  return false;
}

