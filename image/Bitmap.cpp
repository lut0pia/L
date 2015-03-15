#include "Bitmap.h"

#include "../Interface.h"
#include "Vector.h"

using namespace L;
using namespace Image;

Bitmap::Bitmap(size_t width, size_t height) : Array<2,Color>(width,height) {}
Bitmap::Bitmap(size_t width, size_t height, const Image::Vector& v) : Array<2,Color>(width,height) {
  v.drawOn(*this);
}
Bitmap::Bitmap(const String& filePath) : Array<2,Color>() {
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
/*
Color Bitmap::operator()(double x,double y) const{
    Color c;
    int ix = (int)x,
        iy = (int)y;
    double xdp = x - floor(x),
           ydp = y - floor(y);

    Color cTopR = (*this)(ix+1,iy),
            cTopL = (*this)(ix,iy),
            cBotR = (*this)(ix+1,iy+1),
            cBotL = (*this)(ix,iy+1);

    c.r() = ydp*((xdp*(cBotR.r()-cBotL.r())+cBotL.r())
              -(xdp*(cTopR.r()-cTopL.r())+cTopL.r()))
              +(xdp*(cTopR.r()-cTopL.r())+cTopL.r());
    c.g() = ydp*((xdp*(cBotR.g()-cBotL.g())+cBotL.g())
              -(xdp*(cTopR.g()-cTopL.g())+cTopL.g()))
              +(xdp*(cTopR.g()-cTopL.g())+cTopL.g());
    c.b() = ydp*((xdp*(cBotR.b()-cBotL.b())+cBotL.b())
              -(xdp*(cTopR.b()-cTopL.b())+cTopL.b()))
              +(xdp*(cTopR.b()-cTopL.b())+cTopL.b());
    c.a() = ydp*((xdp*(cBotR.a()-cBotL.a())+cBotL.a())
              -(xdp*(cTopR.a()-cTopL.a())+cTopL.a()))
              +(xdp*(cTopR.a()-cTopL.a())+cTopL.a());
    return c;
}
*/
Bitmap Bitmap::sub(size_t x, size_t y, size_t width, size_t height) const {
  Bitmap wtr;
  wtr.resize(width,height);
  for(size_t i=0; i<width; i++)
    for(size_t j=0; j<height; j++)
      wtr(i,j) = (*this)(x+i,y+j);
  return wtr;
}
Bitmap Bitmap::filter(Color c) const {
  Bitmap wtr;
  wtr.resize(width(),height());
  float r = (float)c.r()/255.0,
        g = (float)c.g()/255.0,
        b = (float)c.b()/255.0,
        a = (float)c.a()/255.0;
  for(size_t x=0; x<width(); x++)
    for(size_t y=0; y<height(); y++) {
      Color tmp((*this)(x,y));
      wtr(x,y) = Color((float)tmp.r()*r,
                       (float)tmp.g()*g,
                       (float)tmp.b()*b,
                       (float)tmp.a()*a);
    }
  return wtr;
}
Bitmap Bitmap::trim(Color c) const {
  size_t left = 0, right = width()-1,
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
Bitmap Bitmap::trim(size_t left, size_t right, size_t top, size_t bottom) const {
  Bitmap wtr;
  wtr.resize(right-left,bottom-top);
  for(size_t x=left; x<right; x++)
    for(size_t y=top; y<bottom; y++)
      wtr(x-left,y-top) = (*this)(x,y);
  return wtr;
}
void Bitmap::scale(size_t newWidth, size_t newHeight) {
  double hf = (double)height()/(double)newHeight,
         wf = (double)width()/(double)newWidth;
  Bitmap copy(*this);
  resize(newWidth,newHeight);
  for(size_t x=0; x<newWidth; x++)
    for(size_t y=0; y<newHeight; y++)
      (*this)(x,y) = copy((double)x*wf,(double)y*hf);
}
void Bitmap::blur(int factor) {
  uint rt, gt, bt;
  double pixelCount, m;
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
            m = (double)factor-sqrt(pow((double)(x-i),2)+pow((double)(y-j),2));
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
void Bitmap::drawTriangle(Surface<2,double> s,Color c) {
  Interval<2,double> interval(s.gA(),s.gB());
  interval.add(s.gC());
  Point<2,size_t> p;
  // Double loop testing all pixels inside the interval
  for(p.x() = interval.min().x();
      p.x() < interval.max().x();
      p.x()++)
    for(p.y() = interval.min().y();
        p.y() < interval.max().y();
        p.y()++)
      if(s.contains(p))
        (*this)((size_t)p.x(),(size_t)p.y()) = c;
}
/*
void Bitmap::drawTriangle(s2dD triangle, Bitmap *tex, s2dD texCoord){
    i2dD interval = triangle;
    2dD p, sTexp, ap, ab, ac, tap, tab, tac;

    // Prepare triangle basis vectors, image triangle
    ab.x = triangle.b.x - triangle.a.x;
    ab.y = triangle.b.y - triangle.a.y;
    ac.x = triangle.c.x - triangle.a.x;
    ac.y = triangle.c.y - triangle.a.y;
    // Prepare triangle basis vectors, texture triangle
    tab.x = texCoord.b.x - texCoord.a.x;
    tab.y = texCoord.b.y - texCoord.a.y;
    tac.x = texCoord.c.x - texCoord.a.x;
    tac.y = texCoord.c.y - texCoord.a.y;

    // Double loop testing all pixels inside the interval
    for(p.x = interval.a.x;
        p.x <= interval.b.x;
        p.x += 1.0)
        for(p.y = interval.a.y;
            p.y <= interval.b.y;
            p.y += 1.0)
                if(triangle.contains(p)){
                    ap.x = p.x - triangle.a.x;
                    ap.y = p.y - triangle.a.y;
                    tap.x = -(ac.y*ap.x - ac.x*ap.y)/(ab.y*ac.x - ab.x*ac.y);
                    tap.y = (ab.y*ap.x - ab.x*ap.y)/(ab.y*ac.x - ab.x*ac.y);
                    sTexp.x = (tap.x * tab.x + tap.y * tac.x)+texCoord.a.x;
                    sTexp.y = (tap.x * tab.y + tap.y * tac.y)+texCoord.a.y;
                    sColor(p.x,p.y,tex->gScaledColor(sTexp.x,sTexp.y,INTERPOLATION_LINEAR));
                }
}
*/




