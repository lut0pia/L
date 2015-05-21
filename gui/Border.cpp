#include "Border.h"

using namespace L;
using namespace GUI;

Border::Border(Ref<Base> inner, size_t s, Color c) : Layer(inner), size(s), color(c) {}

Point2i Border::gPos() {
  return inner->gPos()-Point<2,int>(size,size);
}
Interval2i Border::gClip() {
  Interval2i wtr(inner->gClip());
  wtr.add(wtr.min()-Point<2,int>(size,size));
  wtr.add(wtr.max()+Point<2,int>(size,size));
  return wtr;
}
Point2i Border::gDimensions() {
  return inner->gDimensions()+Point<2,int>(size*2,size*2);
}
void Border::dimensionsChanged(Base* e,Point2i newDim) {
  Base::dimensionsChanged(newDim+Point<2,int>(size*2,size*2));
}
void Border::updateFromAbove(Point2i pos,Interval2i parentClip) {
  inner->updateFromAbove(pos+Point<2,int>(size,size),parentClip);
}
void Border::draw(GL::Program& program) {
  Interval2i clip(gClip());
  if(!clip.empty()) {
    /*
    Array<float> vertex(8);
    vertex[0] = clip.gA().x();
    vertex[1] = clip.gA().y();
    vertex[2] = clip.gB().x();
    vertex[3] = clip.gA().y();
    vertex[4] = clip.gB().x();
    vertex[5] = clip.gB().y();
    vertex[6] = clip.gA().x();
    vertex[7] = clip.gB().y();
    Graphics::draw2dColorQuad(vertex,color);
    */
    glColor4ub(color.r(),color.g(),color.b(),color.a());
    glBegin(GL_QUADS);
    glVertex2i(clip.min().x(),clip.min().y());
    glVertex2i(clip.max().x(),clip.min().y());
    glVertex2i(clip.max().x(),clip.max().y());
    glVertex2i(clip.min().x(),clip.max().y());
    glEnd();
    inner->draw(program);
  }
}
