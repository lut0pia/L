#include "Image.h"

#include "../gl/GL.h"

using namespace L;
using namespace GUI;
using L::Image::Bitmap;
using GUI::Image;

GUI::Image::Image() : Solid(), dimensions(0,0) {}
GUI::Image::Image(const Bitmap& bmp) : Solid() {
  sBitmap(bmp);
}
void GUI::Image::sBitmap(const Bitmap& bmp) {
  tex = new GL::Texture(bmp);
  dimensions.x() = bmp.width();
  dimensions.y() = bmp.height();
  dimensionsChanged(dimensions);
}

Point2i GUI::Image::gDimensions() {
  return dimensions;
}
void GUI::Image::draw(GL::Program& program) {
  if(tex && !clip.empty()) {
    program.uniform("texture",*tex);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    glTexCoord2f((clip.min().x()>pos.x()) ? (float)(clip.min().x()-pos.x())/dimensions.x() : 0.f,
                 (clip.min().y()>pos.y()) ? (float)(clip.min().y()-pos.y())/dimensions.y() : 0.f);
    glVertex2i(clip.min().x(),clip.min().y());
    glTexCoord2f((clip.min().x()>pos.x()) ? (float)(clip.min().x()-pos.x())/dimensions.x() : 0.f,
                 (clip.max().y()<pos.y()+dimensions.y()) ? (float)(clip.max().y()-pos.y())/dimensions.y() : 1.f);
    glVertex2i(clip.min().x(),clip.max().y());
    glTexCoord2f((clip.max().x()<pos.x()+dimensions.x()) ? (float)(clip.max().x()-pos.x())/dimensions.x() : 1.f,
                 (clip.max().y()<pos.y()+dimensions.y()) ? (float)(clip.max().y()-pos.y())/dimensions.y() : 1.f);
    glVertex2i(clip.max().x(),clip.max().y());
    glTexCoord2f((clip.max().x()<pos.x()+dimensions.x()) ? (float)(clip.max().x()-pos.x())/dimensions.x() : 1.f,
                 (clip.min().y()>pos.y()) ? (float)(clip.min().y()-pos.y())/dimensions.y() : 0.f);
    glVertex2i(clip.max().x(),clip.min().y());
    glEnd();
    tex->unbind();
  }
}
