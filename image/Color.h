#ifndef DEF_L_Color
#define DEF_L_Color

#include <cstdlib>
#include <iostream>
#include "../macros.h"
#include "../Serializable.h"
#include "../stl/String.h"

enum L_colorformat{
    _cfGS,
    _cfRGB,
    _cfRGBA
};

namespace L{
    class Color : public Serializable{
        protected:
            byte data[4];
        public:
            Color();
            Color(byte gs);
            Color(byte r, byte g, byte b, byte a = 0xFF);

            bool operator==(const Color&) const;
            bool operator!=(const Color&) const;
            bool operator<(const Color&) const;
            Color& operator+=(const Color&);

            byte r() const;
            byte g() const;
            byte b() const;
            byte a() const;
            byte& r();
            byte& g();
            byte& b();
            byte& a();
            const byte* bytes() const{return data;}

            void write(std::ostream&) const;
            void read(std::istream&);

            static Color from(const String&);
            static Color from(float r, float g, float b, float a = 1);
            static Color random();
            static const Color black, blue, green, grey, lightGrey, red, white, yellow;
    };
}

#endif





