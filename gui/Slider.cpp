#include "Slider.h"

using namespace L;
using namespace GUI;

Slider::Slider(Point2i d, Ref<Base> thumb) : Sizable(d), curDist(0,0){
    sThumb(thumb);
    inMotion = false;
}
void Slider::sThumb(Ref<Base> thumb){
    this->thumb = thumb;
    thumb->sParent(this);
    sValue(Point<2,float>(0,0));
}
void Slider::sValue(Point<2,float> v){
    /*
    Point2i thumbDim(thumb->gDimensions());
    v.x *= (float)(dimensions.x()-thumbDim.x());
    v.y *= (float)(dimensions.y()-thumbDim.y());
    */
    thumbPos = (dimensions - thumb->gDimensions()) * v;
    updateThumb();
}

Point<2,float> Slider::gValue() const{
    return value;
}
void Slider::updateThumb(){
    thumb->updateFromAbove(pos+thumbPos,clip);
}
void Slider::updateValue(){
    Point2i thumbDim(thumb->gDimensions());
    value = Point<2,float>((float)thumbPos.x()/(float)(dimensions.x()-thumbDim.x()),
                       (float)thumbPos.y()/(float)(dimensions.x()-thumbDim.y()));
    updateThumb();
}
void Slider::dimensionsChanged(Base* e,Point2i thumbDim){
    Point<2,float> v(value);
    v.x() *= (float)(dimensions.x()-thumbDim.x());
    v.y() *= (float)(dimensions.y()-thumbDim.y());
    thumbPos = v;
    updateThumb();
}
void Slider::updateFromAbove(Point2i pos,Interval2i parentClip){
    Solid::updateFromAbove(pos,parentClip);
    updateThumb();
}
void Slider::draw(){
    thumb->draw();
}
bool Slider::event(const Window::Event& e){
    bool wtr = false;
    if(e.type == Window::Event::LBUTTONDOWN && thumb->gClip().contains(Point<2,int>(e.x,e.y))){
        inMotion = true;
        curDist = Point<2,int>(e.x,e.y) - thumb->gPos();
        wtr = true;
    }
    if(e.type == Window::Event::LBUTTONUP)
        inMotion = false;
    if(inMotion){
        thumbPos = (Interval<2,int>(Point<2,int>(0,0),dimensions-thumb->gDimensions()) // Interval of inside values
                    .closestTo(Point<2,int>(e.x,e.y)-pos-curDist));
        updateValue();
        wtr = true;
    }
    return wtr;
}

