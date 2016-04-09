#include "Sizable.h"

using namespace L;
using namespace L::GUI;

Sizable::Sizable() : Solid(){}
Sizable::Sizable(Vector2i d) : Solid(), dimensions(d){
    clip.add(d);
}
void Sizable::sDimensions(Vector2i d){
    dimensions = d;
    dimensionsChanged(d);
}
Vector2i Sizable::gDimensions(){
    return dimensions;
}
