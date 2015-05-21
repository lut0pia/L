#include "Rational.h"

#include <cstdio>

using namespace L;

void Rational::simplification() {
  Integer gcd(Integer::gcd(a.abs(),b.abs()));
  a /= gcd;
  b /= gcd;
}

Rational::Rational(const long& a) : a(a), b(1) {}
Rational::Rational(const Integer& a, const Integer& b) : a(a), b(b) {
  simplification();
}
Rational::Rational(const String& str, long base) : a(str) {
  size_t point(str.find_first_of('.'));
  b = (point!=String::npos) ? Integer::pow(base,str.size()-point-1) : 1;
  simplification();
}

Rational Rational::operator+(const Rational& cb) const {
  return Rational((a*cb.b)+(cb.a*b),b*cb.b);
}
Rational Rational::operator-(const Rational& cb) const {
  return Rational((a*cb.b)-(cb.a*b),b*cb.b);
}
Rational Rational::operator+() const {
  return Rational(a,b);
}
Rational Rational::operator-() const {
  return Rational(-a,b);
}
Rational Rational::operator*(const Rational& cb) const {
  return Rational(a*cb.a,b*cb.b);
}
Rational Rational::operator/(const Rational& cb) const {
  return Rational(a*cb.b,b*cb.a);
}
Rational Rational::operator%(const Rational& cb) const {
  return *this-(Rational((*this/cb).intval())*cb);
}

Rational& Rational::operator ++() {
  return *this+=1;
}
Rational Rational::operator ++(int) {
  Rational wtr = *this;
  *this+=1;
  return wtr;
}
Rational& Rational::operator --() {
  return *this-=1;
}
Rational Rational::operator --(int) {
  Rational wtr = *this;
  *this-=1;
  return wtr;
}

bool Rational::operator==(const Rational& cb) const {
  return (a*cb.b==b*cb.a);
}
bool Rational::operator!=(const Rational& cb) const {
  return (a!=cb.a||b!=cb.b);
}
bool Rational::operator>(const Rational& cb) const {
  return (a*cb.b)>(cb.a*b);
}
bool Rational::operator<(const Rational& cb) const {
  return (a*cb.b)<(cb.a*b);
}
bool Rational::operator>=(const Rational& cb) const {
  return !(*this<cb);
}
bool Rational::operator<=(const Rational& cb) const {
  return !(*this>cb);
}

Rational& Rational::operator +=(const Rational& cb) {
  return *this = *this+cb;
}
Rational& Rational::operator -=(const Rational& cb) {
  return *this = *this-cb;
}
Rational& Rational::operator *=(const Rational& cb) {
  return *this = *this*cb;
}
Rational& Rational::operator /=(const Rational& cb) {
  return *this = *this/cb;
}
Rational& Rational::operator %=(const Rational& cb) {
  return *this = *this%cb;
}

const Integer& Rational::gA() const {
  return a;
}
const Integer& Rational::gB() const {
  return b;
}
Integer Rational::intval() const {
  return a/b;
}
Rational Rational::abs() const {
  return Rational(a.abs(),b.abs());
}
bool Rational::negative() const {
  return (a._negative!=b._negative);
}
String Rational::toShortString() const {
  if(b==1)
    return a.toShortString();
  else if(a.size()==b.size()) {
    char buffer[64];
    int ratio;
    if(!a._part.size()) return "0";
    sprintf(buffer,"%1.*f",10,(double)a._part.back()/(double)b._part.back());
    switch(ratio = (a._part.size()-1)-(b._part.size()-1)) {
      case 0:
        return ((negative())?"-":"")+String(buffer);
      case 1:
        return ((negative())?"-":"")+String(buffer)+"*(2^32)";
      default:
        return ((negative())?"-":"")+String(buffer)+"*(2^32^"+ToString(ratio)+")";
    }
  } else if(a.size()>b.size())
    return "inf";
  else return "0";
}
String Rational::toString(long lbase) const {
  Rational n(abs());
  unsigned long buff;
  String wtr = intval().toString();
  if(b!=1) { // Decimal part
    size_t i = 0;
    wtr += '.';
    do {
      n *= lbase;
      buff = (n.intval()%lbase).part(0);
      wtr += (char)((buff<10) ? buff+'0' : buff+'W');
    } while(n.gB()!=1 && ++i<10);
  }
  return ((negative())?"-":"")+wtr;
}

