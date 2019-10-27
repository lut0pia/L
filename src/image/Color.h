#pragma once

#include "../math/math.h"
#include "../math/Vector.h"

namespace L {
  class Color : public Vector4b {
  public:
    inline Color() = default;
    inline Color(const Vector4b& v) : Vector4b(v) {}
    inline Color(uint8_t gs) : Vector4b(gs, gs, gs, uint8_t(0xff)) {}
    inline Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) : Vector4b(r, g, b, a) {}
    Color(const char*);

    inline uint8_t r() const { return _c[0]; }
    inline uint8_t g() const { return _c[1]; }
    inline uint8_t b() const { return _c[2]; }
    inline uint8_t a() const { return _c[3]; }
    inline float rf() const { return (float)r()/255.f; }
    inline float gf() const { return (float)g()/255.f; }
    inline float bf() const { return (float)b()/255.f; }
    inline float af() const { return (float)a()/255.f; }
    inline uint32_t rgb() const { return r()<<16|g()<<8|b(); }
    inline uint32_t rgba() const { return r()<<24|g()<<16|b()<<8|a(); }
    inline uint8_t& r() { return _c[0]; }
    inline uint8_t& g() { return _c[1]; }
    inline uint8_t& b() { return _c[2]; }
    inline uint8_t& a() { return _c[3]; }

    static Vector4f to_float_vector(const Color&);
    static Color from(float r, float g, float b, float a = 1);
    static Color from_hsv(float h, float s, float v);
    static Color lerp(Color, Color, float w);
    static const Color black, blue, cyan, green, grey, lightgrey, magenta, red, transparent, white, yellow;

    friend inline Stream& operator<<(Stream& s, const Color& v) { return s << '#' << ntos<16>(v.rgba(), 8); }
    friend inline Stream& operator<(Stream& s, const Color& v) { return s << '#' << ntos<16>(v.rgba(), 8) << '\n'; }
    friend inline Stream& operator>(Stream& s, Color& v) { v = s.word(); return s; }
  };
}


