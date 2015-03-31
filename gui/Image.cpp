#include "Image.h"

#include "../gl/Utils.h"

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
void GUI::Image::draw() {
  if(tex && !clip.empty()) {
    Vector<float> vertex(8), texCoord(8);
    vertex[0] = (float)clip.min().x();
    vertex[1] = (float)clip.min().y();
    vertex[2] = (float)clip.max().x();
    vertex[3] = (float)clip.min().y();
    vertex[4] = (float)clip.max().x();
    vertex[5] = (float)clip.max().y();
    vertex[6] = (float)clip.min().x();
    vertex[7] = (float)clip.max().y();
    texCoord[0] = (clip.min().x()>pos.x()) ? (float)(clip.min().x()-pos.x())/dimensions.x() : 0.f;
    texCoord[1] = (clip.min().y()>pos.y()) ? (float)(clip.min().y()-pos.y())/dimensions.y() : 0.f;
    texCoord[2] = (clip.max().x()<pos.x()+dimensions.x()) ? (float)(clip.max().x()-pos.x())/dimensions.x() : 1.f;
    texCoord[3] = (clip.min().y()>pos.y()) ? (float)(clip.min().y()-pos.y())/dimensions.y() : 0.f;
    texCoord[4] = (clip.max().x()<pos.x()+dimensions.x()) ? (float)(clip.max().x()-pos.x())/dimensions.x() : 1.f;
    texCoord[5] = (clip.max().y()<pos.y()+dimensions.y()) ? (float)(clip.max().y()-pos.y())/dimensions.y() : 1.f;
    texCoord[6] = (clip.min().x()>pos.x()) ? (float)(clip.min().x()-pos.x())/dimensions.x() : 0.f;
    texCoord[7] = (clip.max().y()<pos.y()+dimensions.y()) ? (float)(clip.max().y()-pos.y())/dimensions.y() : 1.f;
    GL::Utils::draw2dTexQuad(vertex,texCoord,*tex);
  }
}
