#include "Rectangle.h"

#include "GUI.h"

using namespace L;
using namespace L::GUI;

Rectangle::Rectangle() : Sizable(), color(Color::black) {}
Rectangle::Rectangle(Point2i d,Color c) : Sizable(d), color(c) {}

void Rectangle::draw(GL::Program& program) {
  glColor4ub(color.r(),color.g(),color.b(),color.a());
  program.uniform("texture",GL::whiteTexture());
  glBegin(GL_QUADS);
  glVertex2i(clip.min().x(),clip.min().y());
  glVertex2i(clip.min().x(),clip.max().y());
  glVertex2i(clip.max().x(),clip.max().y());
  glVertex2i(clip.max().x(),clip.min().y());
  glEnd();
}
