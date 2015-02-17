#include "Image.h"

#include "../gl/Utils.h"

using namespace L;
using namespace GUI;
using L::Image::Bitmap;
using GUI::Image;

GUI::Image::Image() : Solid(), dimensions(0,0){}
GUI::Image::Image(const Bitmap& bmp) : Solid(){
    sBitmap(bmp);
}
void GUI::Image::sBitmap(const Bitmap& bmp){
    tex = new GL::Texture(bmp);
    dimensions.x() = bmp.width();
    dimensions.y() = bmp.height();
    dimensionsChanged(dimensions);
}

Point2i GUI::Image::gDimensions(){
    return dimensions;
}
void GUI::Image::draw(){
    if(tex && !clip.empty()){
        Vector<float> vertex(8), texCoord(8);

        vertex[0] = clip.gA().x();
        vertex[1] = clip.gA().y();
        vertex[2] = clip.gB().x();
        vertex[3] = clip.gA().y();
        vertex[4] = clip.gB().x();
        vertex[5] = clip.gB().y();
        vertex[6] = clip.gA().x();
        vertex[7] = clip.gB().y();

        texCoord[0] = (clip.gA().x()>pos.x()) ? (float)(clip.gA().x()-pos.x())/dimensions.x() : 0.0;
        texCoord[1] = (clip.gA().y()>pos.y()) ? (float)(clip.gA().y()-pos.y())/dimensions.y() : 0.0;
        texCoord[2] = (clip.gB().x()<pos.x()+dimensions.x()) ? (float)(clip.gB().x()-pos.x())/dimensions.x() : 1.0;
        texCoord[3] = (clip.gA().y()>pos.y()) ? (float)(clip.gA().y()-pos.y())/dimensions.y() : 0.0;
        texCoord[4] = (clip.gB().x()<pos.x()+dimensions.x()) ? (float)(clip.gB().x()-pos.x())/dimensions.x() : 1.0;
        texCoord[5] = (clip.gB().y()<pos.y()+dimensions.y()) ? (float)(clip.gB().y()-pos.y())/dimensions.y() : 1.0;
        texCoord[6] = (clip.gA().x()>pos.x()) ? (float)(clip.gA().x()-pos.x())/dimensions.x() : 0.0;
        texCoord[7] = (clip.gB().y()<pos.y()+dimensions.y()) ? (float)(clip.gB().y()-pos.y())/dimensions.y() : 1.0;
        GL::Utils::draw2dTexQuad(vertex,texCoord,*tex);
    }
}
