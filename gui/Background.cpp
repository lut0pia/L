#include "Background.h"

#include "GUI.h"

using namespace L;
using namespace GUI;

Background::Background(const Ref<Base>& inner, Color c) : Layer(inner), color(c) {}

void Background::draw() {
  Interval2i clip(gClip());
  std::cout << clip.min().x() << " " << clip.min().y() << " "
            << clip.max().x() << " " << clip.max().y() << std::endl;
  if(!clip.empty() || true) {
    glColor3ub(color.r(),color.g(),color.b());
    glBegin(GL_QUADS);
    glVertex2i(clip.min().x(),clip.min().y());
    glVertex2i(clip.max().x(),clip.min().y());
    glVertex2i(clip.max().x(),clip.max().y());
    glVertex2i(clip.min().x(),clip.max().y());
    glEnd();
    inner->draw();
  }
}

