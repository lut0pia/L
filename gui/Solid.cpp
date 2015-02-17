#include "Solid.h"

using namespace L;
using namespace GUI;

Solid::Solid() : Base(), pos(0,0), clip(Point<2,int>(0,0),Point<2,int>(0,0)){}
Point2i Solid::gPos(){
    return pos;
}
Interval2i Solid::gClip(){
    return clip;
}
void Solid::updateFromAbove(Point2i pos,Interval2i parentClip){
    Interval2i wantedClip(pos,pos+gDimensions());
    this->pos = pos;
    this->clip = wantedClip * parentClip;
}
