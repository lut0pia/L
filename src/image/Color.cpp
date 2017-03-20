#include "Color.h"

#include "../macros.h"
#include "../text/String.h"

using namespace L;

const Color Color::black(0, 0, 0);
const Color Color::blue(0, 0, 255);
const Color Color::cyan(0, 255, 255);
const Color Color::green(0, 255, 0);
const Color Color::grey(128, 128, 128);
const Color Color::lightgrey(192, 192, 192);
const Color Color::magenta(255, 0, 255);
const Color Color::red(255, 0, 0);
const Color Color::transparent(0, 0, 0, 0);
const Color Color::white(255, 255, 255);
const Color Color::yellow(255, 255, 0);

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
  L_ERRORF("Unknown color: %s",(const char*)str);
}

Color Color::from(float r, float g, float b, float a) {
  return Color(byte(r*255.f), byte(g*255.f), byte(b*255.f), byte(a*255.f));
}
Color Color::fromHSV(float h, float s, float v) {
  if(s <= 0.f) // No saturation is greyscale
    return Color::from(v, v, v);

  if(h >= 360.f) h = 0.f;
  h /= 60.f;
  const uint32_t i(h);
  const float ff(h - i);
  const float p(v * (1.f - s));
  const float q(v * (1.f - (s * ff)));
  const float t(v * (1.f - (s * (1.0 - ff))));

  switch(i) {
    case 0: return Color::from(v, t, p);
    case 1: return Color::from(q, v, p);
    case 2: return Color::from(p, v, t);
    case 3: return Color::from(p, q, v);
    case 4: return Color::from(t, p, v);
    case 5: default: return Color::from(v, p, q);
  }
}
Color Color::lerp(Color a, Color b, float w) {
  const float nw(1.f-w);
  return Color(byte(nw*a.r() + w*b.r()), byte(nw*a.g() + w*b.g()), byte(nw*a.b() + w*b.b()), byte(nw*a.a() + w*b.a()));
}

