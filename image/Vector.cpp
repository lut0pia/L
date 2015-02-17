#include "Vector.h"

using namespace L;
using namespace Image;

Image::Vector::Vector(){

}
Image::Vector::Vector(const XML&){

}
void Image::Vector::drawOn(Image::Bitmap& bmp) const{
    L_Iter(primitives,it) (*it)->drawOn(bmp);
}

