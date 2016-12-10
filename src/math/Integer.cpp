#include "Integer.h"

#include "../macros.h"
#include "math.h"

using namespace L;

Integer::Integer(const char* str,int lbase) : _negative(false) {
  Integer base(lbase);
  for(const char* i(str); *i; i++){
    const char c(*i);
    int v;
    if(c>='a'&&c<='z') // Go from visual char to actual value
      v = c-('a'-10);
    else if(c>='A'&&c<='Z') // Same
      v = c-('A'-10);
    else if(c>='0'&&c<='9') // Same
      v = c-'0';
    else continue; // Erase useless characters
    operator*=(base);
    operator+=(v);
  }
  _negative = (str[0]=='-');
}

Integer Integer::operator-() const {
  Integer wtr(*this);
  wtr._negative = !wtr.negative();
  return wtr;
}

Integer& Integer::operator++() {
  *this += (long)1;
  return *this;
}
Integer Integer::operator++(int) {
  Integer wtr(*this);
  ++*this;
  return wtr;
}
Integer& Integer::operator--() {
  *this -= (long)1;
  return *this;
}
Integer Integer::operator--(int) {
  Integer wtr(*this);
  --*this;
  return wtr;
}

bool Integer::operator==(const Integer& other) const {
  size_t maxp(max(size(),other.size()));
  type zeroTest = 0,equalityTest = 0;
  for(size_t n = 0; n<maxp; n++) {
    if(part(n)==other.part(n)) {
      equalityTest++;
      if(part(n)==0)
        zeroTest++;
    }
  }
  if((negative() != other.negative()) && (zeroTest != maxp))
    return false;
  else if(equalityTest == maxp)
    return true;
  else
    return false;
}
bool Integer::operator<(const Integer& other) const {
  if(negative()==other.negative()) {
    size_t maxp(max(size(),other.size()));
    for(size_t n = maxp-1; n<=maxp; n--) {
      if(part(n)<other.part(n))
        return !negative();
      else if(part(n)>other.part(n))
        return negative();
    }
    return false;
  } else return negative(); // This is negative -> true, b is negative -> false
}

Integer& Integer::operator+=(const Integer& other) {
  if(negative() != other.negative())
    (*this) -= -other;
  else { // Same sign
    size_t maxp(max(size(),other.size()));
    for(size_t n(0); n<maxp; n++) {
      type tmp(part(n));
      part(n,tmp+other.part(n));
      if(tmp>part(n)) {
        int i = 0;
        do {
          i++;
          part(n+i,part(n+i)+1);
        } while(part(n+i)==0);
      }
    }
    trim();
  }
  return *this;
}
Integer& Integer::operator-=(const Integer& other) {
  if(negative() != other.negative())
    (*this) += -other;
  else if((negative() && (*this) > other) || (!negative() && (*this) < other)) // Only subtract absolutely smaller number
    (*this) = -(other-(*this));
  else { // Same sign
    size_t maxp(max(size(),other.size()));
    for(size_t n(0); n<maxp; n++) {
      type tmp(part(n));
      part(n,tmp-other.part(n));
      if(tmp<part(n)) {
        int i(0);
        do {
          i++;
          part(n+i,part(n+i)-1);
        } while(part(n+i)==(type)-1);
      }
    }
    trim();
  }
  return *this;
}
Integer& Integer::operator*=(const Integer& other) {
  Integer x(abs()),y(other.abs());
  bool resNeg(negative()!=other.negative());
  reset(); // Make this zero
  while(y>0) {
    if(y.part(0)&1) {
      (*this) += x;
      y--;
    } else {
      y >>= 1;
      x <<= 1;
    }
  }
  _negative = resNeg;
  trim();
  return *this;
}
Integer& Integer::operator/=(const Integer& other) {
  if(other==0)
    L_ERROR("Trying to divide by zero.");
  Integer a(abs()),b(other.abs());
  if(a < b)
    reset();
  else {
    Integer limDown,limUp(1);
    size_t n(0);
    bool resNeg(negative() != other.negative());
    // Calculating first limits
    while(limUp*b<=a) {
      limUp <<= 1;
      n++;
    }
    limDown = limUp >> 1;
    // Using dichotomy
    for(size_t i = 0; i<n; i++) {
      (*this) = (limDown + limUp) >> 1;
      if((*this)*b <= a)  limDown = (*this);
      else                limUp = (*this);
    }
    _negative = resNeg;
    trim();
  }
  return *this;
}
Integer& Integer::operator%=(const Integer& other) {
  if(other==0)
    L_ERROR("Trying to mod by zero.");
  (*this) -= ((*this/other)*other);
  trim();
  return *this;
}
Integer& Integer::operator<<=(size_t n) {
  for(size_t i(size()); i!=(size_t)-1; i--) {
    type tmp(part(i));
    part(i,tmp<<n);
    part(i+1,part(i+1)|(tmp>>(typebits-n)));
  }
  trim();
  return *this;
}
Integer& Integer::operator>>=(size_t n) {
  for(size_t i(0); i<size(); i++) {
    type tmp(part(i));
    part(i,tmp>>n);
    if(i)part(i-1,part(i-1)|(tmp<<(typebits-n)));
  }
  trim();
  return *this;
}

Integer Integer::abs() const {
  Integer wtr(*this);
  wtr._negative = false;
  return wtr;
}
String Integer::toShortString() const {
  if(!size())
    return "0";
  else{
    String wtr((negative()) ? "-" : "");
    switch(size()){
      case 1: wtr += ntos(_part[0]); break;
      case 2:
        wtr += ntos(_part.back());
        wtr += "*(2^32)";
        break;
      default:
        wtr += ntos(_part.back());
        wtr += "*(2^32^";
        wtr += ntos(size()-1);
        wtr += ")";
        break;
    }
    return wtr;
  }
}
String Integer::toString(long lbase) const {
  Integer base(lbase),n;
  type buff;
  String wtr("");
  switch(lbase) {
    case 2:
      for(size_t i = size()-1; i!=(size_t)-1; i--)
        wtr += ntos<2>(part(i),sizeof(type)*8);
      while(wtr[0]=='0' && wtr.size()>1)
        wtr.erase(0,1);
      break;
    case 16:
      for(size_t i = size()-1; i!=(size_t)-1; i--)
        wtr += ntos<16>(part(i),sizeof(type)*2);
      while(wtr[0]=='0' && wtr.size()>1)
        wtr.erase(0,1);
      break;
    default:
      n = abs();
      do {
        buff = (n%base).part(0);
        wtr = (char)((buff<10) ? buff+'0' : buff+'W') + wtr;
        n /= base;
      } while(n.size()!=0);
      break;
  }
  if(negative()) wtr = '-' + wtr;
  return wtr;
}

Integer::type Integer::part(size_t position) const {
  if(position<size())
    return _part[position];
  else
    return 0;
}
void Integer::part(size_t position,type p) {
  while(size()<=position)
    _part.push(0);
  _part[position] = p;
}
void Integer::reset() {
  _negative = false;
  _part.clear();
}
void Integer::trim() {
  while(size() && _part.back()==0)
    _part.pop();
}

Stream& L::operator<<(Stream &stream,const Integer& v) {
  if(v.size()) {
    if(v._negative)
      stream << '-';
    stream << v._part.back();
    if(v.size()>1) {
      stream << "*(2^32";
      if(v.size()==2)
        stream << ')';
      else
        stream << '^' << (v.size()-1) << ')';
    }
  } else stream << '0';
  return stream;
}
