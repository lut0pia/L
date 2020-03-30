#include "Color.h"

#include "../macros.h"

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

Vector4f Color::to_float_vector(const Color& color) {
  return Vector4f(color.r() / 255.f, color.g() / 255.f, color.b() / 255.f, color.a() / 255.f);
}
Color Color::from(const char* str) {
  const size_t len = strlen(str);
  if(len > 0 && str[0] == '#') { // Hexadecimal color
    const uint32_t hex = ston<16, uint32_t>(str + 1);
    if(len == 4) { // #RGB
      const uint8_t r = (hex >> 8) & 0xf;
      const uint8_t g = (hex >> 4) & 0xf;
      const uint8_t b = hex & 0xf;
      return Color(r + (r << 4), g + (g << 4), b + (b << 4));
    } else if(len == 7) { // #RRGGBB
      return Color(uint8_t(hex >> 16), uint8_t(hex >> 8), uint8_t(hex));
    } else if(len == 9) { // #RRGGBBAA
      return Color(uint8_t(hex >> 24), uint8_t(hex >> 16), uint8_t(hex >> 8), uint8_t(hex));
    }
  }
#define COLOR_NAME(name) else if(!strcmp(str, #name)) return name
  COLOR_NAME(black);
  COLOR_NAME(blue);
  COLOR_NAME(cyan);
  COLOR_NAME(green);
  COLOR_NAME(grey);
  COLOR_NAME(lightgrey);
  COLOR_NAME(magenta);
  COLOR_NAME(red);
  COLOR_NAME(white);
  COLOR_NAME(yellow);
#undef COLOR_NAME
  warning("Cannot derive color from string: '%s'", str);
  return transparent;
}
Color Color::from(float r, float g, float b, float a) {
  return Color(uint8_t(r*255.f), uint8_t(g*255.f), uint8_t(b*255.f), uint8_t(a*255.f));
}
Color Color::from_hsv(float h, float s, float v) {
  if(s <= 0.f) // No saturation is greyscale
    return Color::from(v, v, v);

  if(h >= 1.f) h = 0.f;
  h *= 6.f;
  const uint32_t i((uint32_t)h);
  const float ff(h - i);
  const float p(v * (1.f - s));
  const float q(v * (1.f - (s * ff)));
  const float t(v * (1.f - (s * (1.f - ff))));

  switch(i) {
    case 0: return Color::from(v, t, p);
    case 1: return Color::from(q, v, p);
    case 2: return Color::from(p, v, t);
    case 3: return Color::from(p, q, v);
    case 4: return Color::from(t, p, v);
    case 5: default: return Color::from(v, p, q);
  }
}
Color Color::from_index(uintptr_t index) {
  return from_hsv(fmod(float(index) * 0.618033988749895f, 1.f), 0.5f, 1.f);
}
Color Color::lerp(Color a, Color b, float w) {
  const float nw(1.f - w);
  return Color(uint8_t(nw*a.r() + w*b.r()), uint8_t(nw*a.g() + w*b.g()), uint8_t(nw*a.b() + w*b.b()), uint8_t(nw*a.a() + w*b.a()));
}

