#include "Rational.h"

#include <cstdio>

using namespace L;

void Rational::simplify() {
  const Integer gcd_value(gcd(_a.abs(),_b.abs()));
  _a /= gcd_value;
  _b /= gcd_value;
}

Rational::Rational(const String& str,uint32_t base) : _a(str) {
  const intptr_t point(str.find_first('.'));
  _b = (point>=0) ? pow<Integer>(base,int32_t(str.size()-point-uintptr_t(1))) : 1;
  simplify();
}

String Rational::toShortString() const {
  if(_b==1)
    return _a.toShortString();
  else if(_a.size()==_b.size()) {
    char buffer[64];
    size_t ratio;
    if(!_a._part.size()) return "0";
    sprintf(buffer,"%1.*f",10,(double)_a._part.back()/(double)_b._part.back());
    switch(ratio = (_a._part.size()-1)-(_b._part.size()-1)) {
      case 0:
        return ((negative()) ? "-" : "")+String(buffer);
      case 1:
        return ((negative()) ? "-" : "")+String(buffer)+"*(2^32)";
      default:
        return ((negative()) ? "-" : "")+String(buffer)+"*(2^32^"+String(ntos(ratio))+")";
    }
  } else if(_a.size()>_b.size())
    return "inf";
  else return "0";
}
String Rational::toString(uint32_t lbase) const {
  Rational n(abs());
  unsigned long buff;
  String wtr = intval().toString();
  if(_b!=1) { // Decimal part
    size_t i = 0;
    wtr.push('.');
    do {
      n *= lbase;
      buff = (n.intval()%lbase).part(0);
      wtr.push(((buff<10) ? buff+'0' : buff+'W'));
    } while(n._b!=1 && ++i<10);
  }
  return ((negative()) ? "-" : "")+wtr;
}

