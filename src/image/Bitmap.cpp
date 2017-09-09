#include "Bitmap.h"

#include "../math/Interpolation.h"

using namespace L;

Bitmap::Bitmap(int width,int height,const Color& c) : MultiArray<2,Color>(width,height) {
  for(int x(0); x<width; x++)
    for(int y(0); y<height; y++)
      (*this)(x,y) = c;
}

Color Bitmap::linear(float x,float y) const {
  x -= .5f;
  y -= .5f;
  float weight[2] = {pmod(x,1.f),pmod(y,1.f)};
  int xi((int)floor(x));
  int yi((int)floor(y));
  Vector4f cell[4] = {
    at(xi,yi),
    at(xi,yi+1),
    at(xi+1,yi),
    at(xi+1,yi+1)
  };
  Vector4f tmp(Interpolation<Vector4f,2>::linear(cell,weight));
  return Color((byte)clamp(tmp[0],0.f,255.f),
    (byte)clamp(tmp[1],0.f,255.f),
               (byte)clamp(tmp[2],0.f,255.f),
               (byte)clamp(tmp[3],0.f,255.f));
}

Color Bitmap::cubic(float x,float y) const {
  x -= .5f;
  y -= .5f;
  float weight[2] = {pmod(x,1.f),pmod(y,1.f)};
  int xi((int)floor(x));
  int yi((int)floor(y));
  Vector4f cell[16] = {
    at(xi-1,yi-1),
    at(xi-1,yi),
    at(xi-1,yi+1),
    at(xi-1,yi+2),
    at(xi,yi-1),
    at(xi,yi),
    at(xi,yi+1),
    at(xi,yi+2),
    at(xi+1,yi-1),
    at(xi+1,yi),
    at(xi+1,yi+1),
    at(xi+1,yi+2),
    at(xi+2,yi-1),
    at(xi+2,yi),
    at(xi+2,yi+1),
    at(xi+2,yi+2)
  };
  Vector4f tmp(Interpolation<Vector4f,2>::cubic(cell,weight));
  return Color((byte)clamp(tmp[0],0.f,255.f),
    (byte)clamp(tmp[1],0.f,255.f),
               (byte)clamp(tmp[2],0.f,255.f),
               (byte)clamp(tmp[3],0.f,255.f));
}
Bitmap Bitmap::sub(int x,int y,int width,int height) const {
  Bitmap wtr;
  wtr.resize(width,height);
  for(int i = 0; i<width; i++)
    for(int j = 0; j<height; j++)
      wtr(i,j) = (*this)(x+i,y+j);
  return wtr;
}
void Bitmap::blit(const Bitmap& bmp,int x,int y) {
  size_t linesize(bmp.width()*sizeof(Color));
  for(int j(0); j<bmp.height(); j++)
    memcpy(&operator()(x,j+y),&bmp(0,j),linesize);
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
  int left = 0,right = width()-1,
    top = 0,bottom = height()-1,
    x,y;
  // Left
  y = top;
  while(c == (*this)(left,y)) {
    if(y<bottom) y++;
    else { y = top; left++; }
  }
  // Top
  x = left;
  while(c == (*this)(x,top)) {
    if(x<right) x++;
    else { x = left; top++; }
  }
  // Right
  y = top;
  while(c == (*this)(right,y)) {
    if(y<bottom) y++;
    else { y = top; right--; }
  }
  // Bottom
  x = left;
  while(c == (*this)(x,bottom)) {
    if(x<right) x++;
    else { x = left; bottom--; }
  }
  return trim(left,right+1,top,bottom+1);
}
Bitmap Bitmap::trim(int left,int right,int top,int bottom) const {
  Bitmap wtr;
  wtr.resize(right-left,bottom-top);
  for(int x = left; x<right; x++)
    for(int y = top; y<bottom; y++)
      wtr(x-left,y-top) = (*this)(x,y);
  return wtr;
}
void Bitmap::scale(int newWidth,int newHeight,InterpolationType it) {
  float hf((float)height()/(float)newHeight),
    wf((float)width()/(float)newWidth);
  Bitmap copy(*this);
  resizeFast(newWidth,newHeight);
  switch(it) {
    case Nearest:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.nearest((float)x*wf,(float)y*hf);
      break;
    case Linear:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.linear((float)x*wf,(float)y*hf);
      break;
    default:
    case Cubic:
      for(int x(0); x<newWidth; x++)
        for(int y(0); y<newHeight; y++)
          (*this)(x,y) = copy.cubic((float)x*wf,(float)y*hf);
      break;
  }
}
void Bitmap::blur(int factor) {
  uint32_t rt,gt,bt;
  float pixelCount,m;
  Color c;
  Bitmap copy(*this);
  for(int x = 0; x<(int)width(); x++)
    for(int y = 0; y<(int)height(); y++) {
      pixelCount = 0.0;
      rt = 0;
      gt = 0;
      bt = 0;
      for(int i = x-factor; i<=x+factor; i++)
        for(int j = y-factor; j<=y+factor; j++)
          if(i>=0 && i<(int)width()
             && j>=0 && j<(int)height()) {
            m = (float)factor-sqrt(pow<float>(x-i,2.f)+pow<float>(y-j,2.f));
            if(m>=1) {
              pixelCount += m;
              c = copy(i,j);
              rt += (byte)(c.r()*m);
              gt += (byte)(c.g()*m);
              bt += (byte)(c.b()*m);
            }
          }
      c.r() = (byte)(rt / pixelCount);
      c.g() = (byte)(gt / pixelCount);
      c.b() = (byte)(bt / pixelCount);
      (*this)(x,y) = c;
    }
}
