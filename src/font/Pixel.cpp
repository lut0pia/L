#include "Pixel.h"

#include "../Exception.h"

using namespace L;
using namespace Font;

Pixel::Pixel(int height) : _ratio(height/7.f) {
  _lineheight = (height*11)/7;
}
#define O Color::white
#define _ Color()
#define MAKE_GLYPH(w,h,...)\
  {\
    Color tmp[] = {__VA_ARGS__};\
    Bitmap bmp(tmp,w,h);\
    if(_ratio>1.f)\
      bmp.scale(w*_ratio,h*_ratio,Bitmap::InterpolationType::NEAREST);\
    Glyph tmp2 = {bmp,Vector2i(0,_ratio),(w+1)*_ratio};\
    return tmp2;\
  }
#define MAKE_UPPER_GLYPH(...) MAKE_GLYPH(5,7,__VA_ARGS__)
#define MAKE_LOWER_GLYPH(...) MAKE_GLYPH(3,9,__VA_ARGS__)
Glyph Pixel::loadGlyph(size_t utf32) {
  switch(utf32) {
    case 'A':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'a':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 _,O,_,
                 _,_,O,
                 _,O,O,
                 O,_,O,
                 _,O,O)
    case 0xE0:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 _,_,O,
                 _,O,_,
                 _,_,O,
                 _,O,O,
                 O,_,O,
                 _,O,O)
    case 0xE2:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 O,_,O,
                 _,O,_,
                 _,_,O,
                 _,O,O,
                 O,_,O,
                 _,O,O)
    case 'B':
      MAKE_UPPER_GLYPH(O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,_)
    case 'b':
      MAKE_GLYPH(3,7,
                 O,_,_,
                 O,_,_,
                 O,O,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,_)
    case 'C':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case 'c':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 _,O,_,
                 O,_,O,
                 O,_,_,
                 O,_,O,
                 _,O,_)
    case 0xE7:
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,_,
                       O,_,O,
                       O,_,_,
                       O,_,O,
                       _,O,_,
                       _,_,O,
                       _,O,_)
    case 'D':
      MAKE_UPPER_GLYPH(O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,_)
    case 'd':
      MAKE_GLYPH(3,7,
                 _,_,O,
                 _,_,O,
                 _,O,O,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,_)
    case 'E':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,O)
    case 'e':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 _,O,_,
                 O,_,O,
                 O,O,O,
                 O,_,_,
                 _,O,O)
    case 0xE8:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 _,_,O,
                 _,O,_,
                 O,_,O,
                 O,O,O,
                 O,_,_,
                 _,O,O)
    case 0xE9:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 O,_,_,
                 _,O,_,
                 O,_,O,
                 O,O,O,
                 O,_,_,
                 _,O,O)
    case 0xEA:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 O,_,O,
                 _,O,_,
                 O,_,O,
                 O,O,O,
                 O,_,_,
                 _,O,O)
    case 0xEB:
      MAKE_GLYPH(3,7,
                 _,_,_,
                 O,_,O,
                 _,O,_,
                 O,_,O,
                 O,O,O,
                 O,_,_,
                 _,O,O)
    case 'F':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_)
    case 'f':
      MAKE_GLYPH(2,7,
                 _,O,
                 O,_,
                 O,O,
                 O,_,
                 O,_,
                 O,_,
                 O,_)
    case 'G':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,O,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case 'g':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,_,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       _,O,O,
                       _,_,O,
                       _,O,_)
    case 'H':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'h':
      MAKE_GLYPH(3,7,
                 O,_,_,
                 O,_,_,
                 O,O,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 O,_,O)
    case 'I':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,O,O,O,_)
    case 'i':
      MAKE_GLYPH(1,7,
                 O,
                 _,
                 O,
                 O,
                 O,
                 O,
                 O)
    case 'J':
      MAKE_UPPER_GLYPH(_,O,O,O,O,
                       _,_,_,O,_,
                       _,_,_,O,_,
                       _,_,_,O,_,
                       _,_,_,O,_,
                       O,_,_,O,_,
                       _,O,O,_,_)
    case 'j':
      MAKE_GLYPH(2,9,
                 _,O,
                 _,_,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 O,_)
    case 'K':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,O,_,
                       O,_,O,_,_,
                       O,O,_,_,_,
                       O,_,O,_,_,
                       O,_,_,O,_,
                       O,_,_,_,O)
    case 'k':
      MAKE_GLYPH(3,7,
                 O,_,_,
                 O,_,_,
                 O,_,O,
                 O,_,O,
                 O,O,_,
                 O,_,O,
                 O,_,O)
    case 'L':
      MAKE_UPPER_GLYPH(O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,O)
    case 'l':
      MAKE_GLYPH(1,7,
                 O,
                 O,
                 O,
                 O,
                 O,
                 O,
                 O)
    case 'M':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,O,_,O,O,
                       O,_,O,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'm':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,_,_,_,
                       _,O,_,O,_,
                       O,_,O,_,O,
                       O,_,O,_,O,
                       O,_,O,_,O,
                       O,_,_,_,O)
    case 'N':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,_,_,O,
                       O,_,O,_,O,
                       O,_,_,O,O,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'n':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 O,O,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 O,_,O)
    case 'O':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case 'o':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 _,O,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,_)
    case 0xF4:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 O,_,O,
                 _,O,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,_)
    case 'P':
      MAKE_UPPER_GLYPH(O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,_,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,_,_,_,_)
    case 'p':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,_,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       O,O,_,
                       O,_,_,
                       O,_,_)
    case 'Q':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,O,_,O,
                       O,_,_,O,O,
                       _,O,O,O,O)
    case 'q':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,_,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       _,O,O,
                       _,_,O,
                       _,_,O)
    case 'R':
      MAKE_UPPER_GLYPH(O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'r':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,_,
                       O,_,O,
                       O,_,_,
                       O,_,_,
                       O,_,_,
                       _,_,_,
                       _,_,_)
    case 'S':
      MAKE_UPPER_GLYPH(_,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       _,O,O,O,_,
                       _,_,_,_,O,
                       _,_,_,_,O,
                       O,O,O,O,_)
    case 's':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       _,O,O,
                       O,_,_,
                       _,O,_,
                       _,_,O,
                       O,O,_,
                       _,_,_,
                       _,_,_)
    case 'T':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_)
    case 't':
      MAKE_GLYPH(2,7,
                 O,_,
                 O,_,
                 O,O,
                 O,_,
                 O,_,
                 O,_,
                 _,O)
    case 'U':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case 'u':
      MAKE_GLYPH(3,7,
                 _,_,_,
                 _,_,_,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,O)
    case 0xF9:
      MAKE_GLYPH(3,7,
                 _,O,_,
                 _,_,O,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 O,_,O,
                 _,O,O)
    case 'V':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,_,O,_,
                       _,_,O,_,_)
    case 'v':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       _,O,_,
                       _,_,_,
                       _,_,_)
    case 'W':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,O,_,O,
                       _,O,_,O,_)
    case 'w':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,_,_,_,
                       O,_,_,_,O,
                       O,_,O,_,O,
                       O,_,O,_,O,
                       O,_,O,_,O,
                       _,O,_,O,_)
    case 'X':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,_,O,_,
                       _,_,O,_,_,
                       _,O,_,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O)
    case 'x':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       O,_,O,
                       O,_,O,
                       _,O,_,
                       O,_,O,
                       O,_,O,
                       _,_,_,
                       _,_,_)
    case 'Y':
      MAKE_UPPER_GLYPH(O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,_,O,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_)
    case 'y':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       O,_,O,
                       _,O,O,
                       _,_,O,
                       _,O,_)
    case 'Z':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       _,_,_,_,O,
                       _,_,_,O,_,
                       _,_,O,_,_,
                       _,O,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,O)
    case 'z':
      MAKE_LOWER_GLYPH(_,_,_,
                       _,_,_,
                       O,O,O,
                       _,_,O,
                       _,O,_,
                       O,_,_,
                       O,O,O,
                       _,_,_,
                       _,_,_)
    case '0':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,O,O,
                       O,_,O,_,O,
                       O,O,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '1':
      MAKE_UPPER_GLYPH(_,_,O,_,_,
                       _,O,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,O,O,O,_)
    case '2':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       _,_,_,_,O,
                       _,_,_,O,_,
                       _,_,O,_,_,
                       _,O,_,_,_,
                       O,O,O,O,O)
    case '3':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       _,_,_,_,O,
                       _,O,O,O,_,
                       _,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '4':
      MAKE_UPPER_GLYPH(_,_,_,O,_,
                       _,_,O,O,_,
                       _,O,_,O,_,
                       O,_,_,O,_,
                       O,O,O,O,O,
                       _,_,_,O,_,
                       _,_,_,O,_)
    case '5':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       O,_,_,_,_,
                       O,_,_,_,_,
                       O,O,O,O,_,
                       _,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '6':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,_,
                       O,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '7':
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       _,_,_,_,O,
                       _,_,_,O,_,
                       _,_,_,O,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_)
    case '8':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '9':
      MAKE_UPPER_GLYPH(_,O,O,O,_,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,O,
                       _,_,_,_,O,
                       O,_,_,_,O,
                       _,O,O,O,_)
    case '!':
      MAKE_GLYPH(1,7,
                 O,
                 O,
                 O,
                 O,
                 O,
                 _,
                 O)
    case '?':
      MAKE_GLYPH(3,7,
                 _,O,_,
                 O,_,O,
                 O,_,O,
                 _,_,O,
                 _,O,_,
                 _,_,_,
                 _,O,_)
    case '.':
      MAKE_GLYPH(1,7,
                 _,
                 _,
                 _,
                 _,
                 _,
                 _,
                 O)
    case ':':
      MAKE_GLYPH(1,7,
                 _,
                 _,
                 _,
                 O,
                 _,
                 _,
                 O)
    case ';':
      MAKE_GLYPH(2,8,
                 _,_,
                 _,_,
                 _,_,
                 _,O,
                 _,_,
                 _,_,
                 _,O,
                 O,_)
    case '=':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,_,_,_,
                       O,O,O,O,O,
                       _,_,_,_,_,
                       _,_,_,_,_,
                       O,O,O,O,O,
                       _,_,_,_,_)
    case '*':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,O,_,_,
                       O,_,O,_,O,
                       _,O,O,O,_,
                       O,_,O,_,O,
                       _,_,O,_,_,
                       _,_,_,_,_)
    case '/':
      MAKE_UPPER_GLYPH(_,_,_,O,_,
                       _,_,_,O,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,O,_,_,_,
                       _,O,_,_,_)
    case '+':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       O,O,O,O,O,
                       _,_,O,_,_,
                       _,_,O,_,_,
                       _,_,_,_,_)
    case '-':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,_,_,_,
                       _,_,_,_,_,
                       O,O,O,O,O,
                       _,_,_,_,_,
                       _,_,_,_,_,
                       _,_,_,_,_)
    case ',':
      MAKE_GLYPH(2,8,
                 _,_,
                 _,_,
                 _,_,
                 _,_,
                 _,_,
                 _,_,
                 _,O,
                 O,_)
    case '\'':
      MAKE_GLYPH(1,2,
                 O,
                 O)
    case '"':
      MAKE_GLYPH(3,2,
                 O,_,O,
                 O,_,O)
    case '(':
      MAKE_GLYPH(2,7,
                 _,O,
                 O,_,
                 O,_,
                 O,_,
                 O,_,
                 O,_,
                 _,O)
    case ')':
      MAKE_GLYPH(2,7,
                 O,_,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 O,_)
    case '{':
      MAKE_GLYPH(3,7,
                 _,_,O,
                 _,O,_,
                 _,O,_,
                 O,_,_,
                 _,O,_,
                 _,O,_,
                 _,_,O)
    case '}':
      MAKE_GLYPH(3,7,
                 O,_,_,
                 _,O,_,
                 _,O,_,
                 _,_,O,
                 _,O,_,
                 _,O,_,
                 O,_,_)
    case '[':
      MAKE_GLYPH(2,7,
                 O,O,
                 O,_,
                 O,_,
                 O,_,
                 O,_,
                 O,_,
                 O,O)
    case ']':
      MAKE_GLYPH(2,7,
                 O,O,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 _,O,
                 O,O)
    case '_':
      MAKE_GLYPH(5,8,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 _,_,_,_,_,
                 O,O,O,O,O)
    case '@':
      MAKE_GLYPH(7,9,
                 _,_,O,O,O,_,_,
                 _,O,_,_,_,O,_,
                 O,_,_,O,_,_,O,
                 O,_,_,_,O,_,O,
                 O,_,_,O,O,_,O,
                 O,_,O,_,O,_,O,
                 O,_,_,O,O,O,_,
                 _,O,_,_,_,_,_,
                 _,_,O,O,O,O,_)
    case '&':
      MAKE_GLYPH(6,7,
                 _,_,_,_,_,_,
                 _,_,O,_,_,_,
                 _,O,_,O,_,_,
                 _,_,O,_,_,_,
                 _,O,_,O,_,O,
                 O,_,_,_,O,_,
                 _,O,O,O,_,O)
    case '~':
      MAKE_UPPER_GLYPH(_,_,_,_,_,
                       _,_,_,_,_,
                       _,O,_,_,_,
                       O,_,O,_,O,
                       _,_,_,O,_,
                       _,_,_,_,_,
                       _,_,_,_,_)
    case ' ':
      MAKE_GLYPH(3,1,_,_,_)
    default:
      MAKE_UPPER_GLYPH(O,O,O,O,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,_,_,_,O,
                       O,O,O,O,O)
  }
}
#undef O
#undef _
