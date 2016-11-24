#include "Color.h"

#include "../macros.h"
#include "../text/String.h"

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

Color::Color(const char* s) {
  String str(s);
  str.toLower();
  if(str.empty()) {}
  else if(str[0]=='#') { // Hexa color
    if(str.size()==7) { // #RRBBGG
      uint32_t rgb(ston<16,uint32_t>(str.substr(1)));
      new(this)Color(rgb>>16,rgb>>8,rgb);
      return;
    } else if(str.size()==9) { // #RRGGBBAA
      uint32_t rgba(ston<16,uint32_t>(str.substr(1)));
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
  L_ERROR("Unknown color: %s",(const char*)str);
}

Color Color::from(float r, float g, float b, float a) {
  return Color((byte)(r*255.f),(byte)(g*255.f),(byte)(b*255.f),(byte)(a*255.f));
}
Color Color::lerp(Color a, Color b, float w) {
  float nw(1.f-w);
  return Color((byte)(nw*a.r() + w*b.r()),(byte)(w*a.g() + w*b.g()),(byte)(w*a.b() + w*b.b()),(byte)(w*a.a() + w*b.a()));
}

