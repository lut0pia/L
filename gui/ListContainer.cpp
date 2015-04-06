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
  elements.push_back(e);
  e->sParent(this);
  update();
}
void ListContainer::pop_back(size_t n) {
  while(n--)
    elements.pop_back();
  update();
}
size_t ListContainer::size() {
  return elements.size();
}
void ListContainer::clear() {
  elements.clear();
  update();
}

Point2i ListContainer::gDimensions() {
  return dimensions;
}
void ListContainer::dimensionsChanged(Base* e, Point2i newDim) {
  update();
}
void ListContainer::updateFromAbove(Point2i pos, Interval2i parentClip) {
  this->pos = pos;
  clip = parentClip;
  if(!blockInsert) {
    Point2i oldDim(dimensions);
    dimensions.x() = dimensions.y() = 0;
    L_Iter(elements,it) {
      (*it)->updateFromAbove(Point<2,int>(pos.x(),pos.y()+dimensions.y()),parentClip);
      Point2i childDim((*it)->gDimensions());
      dimensions.y() += childDim.y();
      if(it!=--elements.end())
        dimensions.y() += spacing;
      dimensions.x() = std::max(dimensions.x(), childDim.x());
    }
    if(dimensions != oldDim)
      Base::dimensionsChanged(dimensions);
  }
}
void ListContainer::draw(GL::Program& program) {
  L_Iter(elements,it)
  (*it)->draw(program);
}
bool ListContainer::event(const Window::Event& e) {
  L_Iter(elements,it)
  if((*it)->event(e))
    return true;
  return false;
}

