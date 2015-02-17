#include "Rectangle.h"

#include "GUI.h"

using namespace L;
using namespace L::GUI;

Rectangle::Rectangle() : Sizable(), color(Color::black){}
Rectangle::Rectangle(Point2i d,Color c) : Sizable(d), color(c){}

void Rectangle::draw(){
    if(!clip.empty()){
        /*
        Vector<float> vertex(8);
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
        glColor3ub(color.r(),color.g(),color.b());
        glBegin(GL_QUADS);
            glVertex2i(clip.gA().x(),clip.gA().y());
            glVertex2i(clip.gB().x(),clip.gA().y());
            glVertex2i(clip.gB().x(),clip.gB().y());
            glVertex2i(clip.gA().x(),clip.gB().y());
        glEnd();
    }
}
