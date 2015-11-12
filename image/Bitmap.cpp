#include "Bitmap.h"

#include "../Interface.h"
#include "../math/Interpolation.h"
#include <cmath>

using namespace L;

Bitmap::Bitmap(int width, int height, const Color& c) : MultiArray<2,Color>(width,height) {
  for(int x(0); x<width; x++)
    for(int y(0); y<height; y++)
      (*this)(x,y) = c;
}
Bitmap::Bitmap(const String& filePath) : MultiArray<2,Color>() {
  Interface<Bitmap>::fromFile(*this,filePath);
}
Bitmap& Bitmap::load(const String& filePath) {
  Interface<Bitmap>::fromFile(*this,filePath);
  return *this;
}
Bitmap& Bitmap::save(const String& filePath) {
  Interface<Bitmap>::toFile(*this,filePath);
  return *this;
}
const Color& Bitmap::at(int x, int y) const {
  static Color outside;
  if(x<0 || y<0 || x>=width() || y>=height())
    return outside;
  else return MultiArray<2,Color>::operator()(x,y);
}

Color Bitmap::linear(float x, float y) const {
  x-= .5f;
  y-= .5f;
  float weight[2] = {pmod(x,1.f),pmod(y,1.f)};
  x = floor(x);
  y = floor(y);
  Vector4f cell[4] = {
    at(x,y),
    at(x,y+1),
    at(x+1,y),
    at(x+1,y+1)
  };
  Vector4f tmp(Interpolation<Vector4f>::linear<2>(cell,weight));
  return Color(clamp(tmp[0],0.f,255.f),clamp(tmp[1],0.f,255.f),clamp(tmp[2],0.f,255.f),clamp(tmp[3],0.f,255.f));
}

Color Bitmap::cubic(float x, float y) const {
  x-= .5f;
  y-= .5f;
  float weight[2] = {pmod(x,1.f),pmod(y,1.f)};
  x = floor(x);
  y = floor(y);
  Vector4f cell[16] = {
    at(x-1,y-1),
    at(x-1,y),
    at(x-1,y+1),
    at(x-1,y+2),
    at(x,y-1),
    at(x,y),
    at(x,y+1),
    at(x,y+2),
    at(x+1,y-1),
    at(x+1,y),
    at(x+1,y+1),
    at(x+1,y+2),
    at(x+2,y-1),
    at(x+2,y),
    at(x+2,y+1),
    at(x+2,y+2)
  };
  Vector4f tmp(Interpolation<Vector4f>::cubic<2>(cell,weight));
  return Color(clamp(tmp[0],0.f,255.f),clamp(tmp[1],0.f,255.f),clamp(tmp[2],0.f,255.f),clamp(tmp[3],0.f,255.f));
}
Bitmap Bitmap::sub(int x, int y, int width, int height) const {
  Bitmap wtr;
  wtr.resize(width,height);
  for(int i=0; i<width; i++)
    for(int j=0; j<height; j++)
      wtr(i,j) = (*this)(x+i,y+j);
  return wtr;
}
void Bitmap::blit(const Bitmap& bmp, int x, int y) {
  for(int i(0); i<bmp.width(); i++)
    for(int j(0); j<bmp.height(); j++)
      operator()(i+x,j+y) = bmp(i,j);
}
void Bitmap::filter(Color c) {
  for(int x(0); x<width(); x++)
    for(int y(0); y<height(); y++) {
      Color tmp(operator()(x,y));
      if(tmp.r()>c.r()) tmp.r() = c.r();
      if(tmp.g()>c.g()) tmp.g() = c.g();
      if(tmp.b()>c.b()) tmp.b() = c.b();
      if(tmp.a()>c.a()) tmp.a() = c.a();
      operator()(x,y) = tmp;
    }
}
Bitmap Bitmap::trim(Color c) const {
  int left = 0, right = width()-1,
      top = 0, bottom = height()-1,
      x,y;
  // Left
  y = top;
  while(c == (*this)(left,y)) {
    if(y<bottom) y++;
    else {y = top; left++;}
  }
  // Top
  x = left;
  while(c == (*this)(x,top)) {
    if(x<right) x++;
    else {x = left; top++;}
  }
  // Right
  y = top;
  while(c == (*this)(right,y)) {
    if(y<bottom) y++;
    else {y = top; right--;}
  }
  // Bottom
  x = left;
  while(c == (*this)(x,bottom)) {
    if(x<right) x++;
    else {x = left; bottom--;}
  }
  return trim(left,right+1,top,bottom+1);
}
Bitmap Bitmap::trim(int left, int right, int top, int bottom) const {
  Bitmap wtr;
  wtr.resize(right-left,bottom-top);
  for(int x=left; x<right; x++)
    for(int y=top; y<bottom; y++)
      wtr(x-left,y-top) = (*this)(x,y);
  return wtr;
}
void Bitmap::scale(int newWidth, int newHeight, InterpolationType it) {
  float hf((float)height()/(float)newHeight),
        wf((float)width()/(float)newWidth);
  Bitmap copy(*this);
  resizeFast(newWidth,newHeight);
  switch(it) {
    case NEAREST:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.nearest((float)x*wf,(float)y*hf);
      break;
    case LINEAR:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.linear((float)x*wf,(float)y*hf);
      break;
    default:
    case CUBIC:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.cubic((float)x*wf,(float)y*hf);
      break;
  }
}
void Bitmap::blur(int factor) {
  uint rt, gt, bt;
  float pixelCount, m;
  Color c;
  Bitmap copy(*this);
  for(int x = 0; x<(int)width(); x++)
    for(int y = 0; y<(int)height(); y++) {
      pixelCount = 0.0;
      rt = 0;
      gt = 0;
      bt = 0;
      for(int i=x-factor; i<=x+factor; i++)
        for(int j=y-factor; j<=y+factor; j++)
          if(i>=0 && i<(int)width()
              && j>=0 && j<(int)height()) {
            m = (float)factor-sqrt(pow((float)(x-i),2)+pow((float)(y-j),2));
            if(m>=1) {
              pixelCount+=m;
              c = copy(i,j);
              rt += c.r()*m;
              gt += c.g()*m;
              bt += c.b()*m;
            }
          }
      c.r() = rt / pixelCount;
      c.g() = gt / pixelCount;
      c.b() = bt / pixelCount;
      (*this)(x,y) = c;
    }
}
void Bitmap::drawTriangle(Surface<2,float> s,Color c) {
  Interval<2,float> interval(s.gA(),s.gB());
  interval.add(s.gC());
  Vector<2,int> p;
  // float loop testing all pixels inside the interval
  for(p.x() = interval.min().x();
      p.x() < interval.max().x();
      p.x()++)
    for(p.y() = interval.min().y();
        p.y() < interval.max().y();
        p.y()++)
      if(s.contains(p))
        (*this)((int)p.x(),(int)p.y()) = c;
}
