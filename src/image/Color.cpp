#include "Color.h"

#include "../macros.h"

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

Color::Color(String str) {
  str.toLower();
  if(str.empty()) {}
  else if(str[0]=='#') { // Hexa color
    if(str.size()==7) { // #RRBBGG
      uint rgb(stringToNumber<16,uint>(str.substr(1)));
      new(this)Color(rgb>>16,rgb>>8,rgb);
      return;
    } else if(str.size()==9) { // #RRGGBBAA
      uint rgba(stringToNumber<16,uint>(str.substr(1)));
      new(this)Color(rgba>>24,rgba>>16,rgba>>8,rgba);
      return;
    }
  }
#define TMP(name) else if(str==#name){*this = name; return;}
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
  L_ERROR("Unknown color: "+str);
}

Color Color::from(float r, float g, float b, float a) {
  return Color(r*255,g*255,b*255,a*255);
}
Color Color::lerp(Color a, Color b, float w) {
  float nw(1.f-w);
  return Color(nw*a.r() + w*b.r(), w*a.g() + w*b.g(), w*a.b() + w*b.b(), w*a.a() + w*b.a());
}

