#include "Fixed.h"

#include <cmath>

using namespace L;

Fixed::Fixed(double f) {
  double g;
  f = modf(f, &g);
  _raw = (int32_t(g)<<halfbits);
  if(f!=0) _raw |= int32_t(f*0xffff+1);
}
Stream& L::operator<<(Stream& s, const Fixed& v) {
  int r(v.raw());
  if(r<0) {
    r = 1+~r;
    s << '-';
  }
  s << (r>>Fixed::halfbits);
  r &= 0xffff;
  if(r) {
    s << '.';
    for(int i(0); i<6&&r; i++) {
      r *= 10;
      s << char('0'+(r>>Fixed::halfbits));
      r &= 0xffff;
    }
  }
  return s;
}
