#include "RelativeContainer.h"

using namespace L;
using namespace L::GUI;

//const Vector<2,float> GUI::TL(0,0),GUI::CL(0,.5),GUI::BL(0,1),GUI::TC(.5,0),GUI::CC(.5,.5),GUI::BC(.5,1),GUI::TR(1,0),GUI::CR(1,.5),GUI::BR(1,1);

RelativeContainer::RelativeContainer() : Sizable() {}
RelativeContainer::RelativeContainer(const Vector<2,int>& d) : Sizable(d) {}

void RelativeContainer::place(const Ref<Base>& e, const Vector<2,int>& relPixPos, const Vector<2,float>& srcRatPos, const Vector<2,float>& tarRatPos) {
  e->sParent(this);
  positions[(Base*)e] = {relPixPos,srcRatPos,tarRatPos};
  elements.insert(e);
  dimensionsChanged((Base*)e,e->gDimensions());
}
void RelativeContainer::detach(Ref<Base> e) {
  e->sParent(nullptr);
  elements.erase(e);
}
void RelativeContainer::clear() {
  elements.clear();
  positions.clear();
}

void RelativeContainer::dimensionsChanged(Base* e,Vector2i newDim) {
  RelPos rp = positions[e];
  Vector2i childRPP(rp.relPixPos);
  Vector<2,float> childSRP(rp.srcRatPos);
  Vector<2,float> childTRP(rp.tarRatPos);
  e->updateFromAbove(Vector<2,int>(pos.x() + (childSRP.x() * dimensions.x()) - (childTRP.x() * newDim.x()) + childRPP.x(),
                                  pos.y() + (childSRP.y() * dimensions.y()) - (childTRP.y() * newDim.y()) + childRPP.y()),
                     clip);
}
void RelativeContainer::updateFromAbove(Vector2i pos, Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  for(auto&& e : positions)
    dimensionsChanged(e.key(),e.key()->gDimensions());
}
void RelativeContainer::draw(GL::Program& program) {
  for(auto&& element : elements)
    element->draw(program);
}
bool RelativeContainer::event(const Window::Event& e) {
  for(auto&& element : elements)
    if(element->event(e))
      return true;
  return false;
}
