#include "Background.h"

#include "GUI.h"

using namespace L;
using namespace GUI;

Background::Background(const Ref<Base>& inner, Color c) : Layer(inner), color(c){}

void Background::draw(){
    Interval2i clip(gClip());
    std::cout << clip.gA().x() << " " << clip.gA().y() << " "
              << clip.gB().x() << " " << clip.gB().y() << std::endl;
    if(!clip.empty() || true){
        glColor3ub(color.r(),color.g(),color.b());
        glBegin(GL_QUADS);
            glVertex2i(clip.gA().x(),clip.gA().y());
            glVertex2i(clip.gB().x(),clip.gA().y());
            glVertex2i(clip.gB().x(),clip.gB().y());
            glVertex2i(clip.gA().x(),clip.gB().y());
        glEnd();
        inner->draw();
    }
}

