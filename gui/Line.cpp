#include "Line.h"

#include "GUI.h"

using namespace L;
using namespace GUI;

Line::Line(Point2i d, Color c) : Sizable(d), c(c), visible(true){}

void Line::updateFromAbove(Point2i pos, Interval2i parentClip){
    Solid::updateFromAbove(pos,parentClip);
    a = pos;
    b = pos+dimensions;
    visible = (parentClip.contains(a) && parentClip.contains(b));
}
void Line::draw(){
    if(visible){
        //Graphics::draw2dLine(a,b,1,c);
        glColor3ub(c.r(),c.g(),c.b());

        glBegin(GL_LINES);
            glVertex2i(a.x(),a.y());
            glVertex2i(b.x(),b.y());
        glEnd();
    }
}

