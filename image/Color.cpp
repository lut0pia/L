#include "Color.h"

#include <cstring>
#include "../bytes.h"
#include "../Exception.h"
#include "../Rand.h"

using namespace L;

const Color Color::black(0,0,0);
const Color Color::blue(0,0,255);
const Color Color::cyan(0,255,255);
const Color Color::green(0,255,0);
const Color Color::grey(128,128,128);
const Color Color::lightgrey(192,192,192);
const Color Color::magenta(255,0,255);
const Color Color::red(255,0,0);
const Color Color::white(255,255,255);
const Color Color::yellow(255,255,0);

Color::Color() {
  _data[0] =
    _data[1] =
      _data[2] =
        _data[3] = 0;
}
Color::Color(byte gs) {
  _data[0] =
    _data[1] =
      _data[2] = gs;
  _data[3] = 0xFF;
}
Color::Color(byte r, byte g, byte b, byte a) {
  _data[0] = r;
  _data[1] = g;
  _data[2] = b;
  _data[3] = a;
}

bool Color::operator==(const Color& other) const {
  return !memcmp(_data,other._data,4);
}
bool Color::operator!=(const Color& other) const {
  return !(*this == other);
}
bool Color::operator<(const Color& other) const {
  for(uint i(0); i<4; i++)
    if(_data[i]<other._data[i])
      return true;
    else if(_data[i]>other._data[i])
      return false;
  return false;
}
Color& Color::operator+=(const Color& other) {
  for(size_t i=0; i<4; i++) {
    if((_data[i]+other._data[i])%0xFF < _data[i])
      _data[i] = 0xFF;
    else
      _data[i] += other._data[i];
  }
  return *this;
}

Color Color::from(String str) {
  str.toLower();
  if((str.size()==7 || str.size()==9) && str[0]=='#') { // Hexa color
    Array<byte> tmp(htb(str.substr(1)));
    if(tmp.size()<4)
      tmp.push(255);
    return Color(tmp[0],tmp[1],tmp[2],tmp[3]);
  }
#define TMP(name) else if(str==#name) return name;
  TMP(black)
  TMP(blue)
  TMP(cyan)
  TMP(green)
  TMP(grey)
  TMP(lightgrey)
  TMP(magenta)
  TMP(red)
  TMP(white)
  TMP(yellow)
#undef TMP
  else throw Exception("Unknown color: "+str);
}
Color Color::from(float r, float g, float b, float a) {
  return Color(r*255,g*255,b*255,a*255);
}
Color Color::lerp(Color a, Color b, float w) {
  float nw(1.f-w);
  return Color(nw*a.r() + w*b.r(), w*a.g() + w*b.g(), w*a.b() + w*b.b(), w*a.a() + w*b.a());
}

