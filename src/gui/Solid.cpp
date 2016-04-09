#include "Solid.h"

using namespace L;
using namespace GUI;

Solid::Solid() : Base(), pos(0,0), clip(Vector<2,int>(0,0),Vector<2,int>(0,0)){}
Vector2i Solid::gPos(){
    return pos;
}
Interval2i Solid::gClip(){
    return clip;
}
void Solid::updateFromAbove(Vector2i pos,Interval2i parentClip){
    Interval2i wantedClip(pos,pos+gDimensions());
    this->pos = pos;
    this->clip = wantedClip * parentClip;
}
