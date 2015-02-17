#include "Sizable.h"

using namespace L;
using namespace L::GUI;

Sizable::Sizable() : Solid(){}
Sizable::Sizable(Point2i d) : Solid(), dimensions(d){
    clip.add(d);
}
void Sizable::sDimensions(Point2i d){
    dimensions = d;
    dimensionsChanged(d);
}
Point2i Sizable::gDimensions(){
    return dimensions;
}
