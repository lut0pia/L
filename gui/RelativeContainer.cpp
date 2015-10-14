#include "RelativeContainer.h"

using namespace L;
using namespace L::GUI;

//const Point<2,float> GUI::TL(0,0),GUI::CL(0,.5),GUI::BL(0,1),GUI::TC(.5,0),GUI::CC(.5,.5),GUI::BC(.5,1),GUI::TR(1,0),GUI::CR(1,.5),GUI::BR(1,1);

RelativeContainer::RelativeContainer() : Sizable() {}
RelativeContainer::RelativeContainer(const Point<2,int>& d) : Sizable(d) {}

void RelativeContainer::place(const Ref<Base>& e, const Point<2,int>& relPixPos, const Point<2,float>& srcRatPos, const Point<2,float>& tarRatPos) {
  e->sParent(this);
  positions[(Base*)e] = {relPixPos,srcRatPos,tarRatPos};
  elements.insert(e);
  dimensionsChanged((Base*)e,e->gDimensions());
}
void RelativeContainer::detach(Ref<Base> e) {
  e->sParent(NULL);
  elements.erase(e);
}
void RelativeContainer::clear() {
  elements.clear();
  positions.clear();
}

void RelativeContainer::dimensionsChanged(Base* e,Point2i newDim) {
  RelPos rp = positions[e];
  Point2i childRPP(rp.relPixPos);
  Point<2,float> childSRP(rp.srcRatPos);
  Point<2,float> childTRP(rp.tarRatPos);
  e->updateFromAbove(Point<2,int>(pos.x() + (childSRP.x() * dimensions.x()) - (childTRP.x() * newDim.x()) + childRPP.x(),
                                  pos.y() + (childSRP.y() * dimensions.y()) - (childTRP.y() * newDim.y()) + childRPP.y()),
                     clip);
}
void RelativeContainer::updateFromAbove(Point2i pos, Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  positions.foreach([this](const KeyValue<Base*,RelPos>& e) {
    dimensionsChanged(e.key(),e.key()->gDimensions());
  });
}
void RelativeContainer::draw(GL::Program& program) {
  for(int i(0); i<elements.size(); i++)
    elements[i]->draw(program);
}
bool RelativeContainer::event(const Window::Event& e) {
  for(int i(0); i<elements.size(); i++)
    if(elements[i]->event(e))
      return true;
  return false;
}
