#include "Integer.h"

#include "../bytes.h"
#include "../Exception.h"
#include "../general.h"
#include "../macros.h"

using namespace L;

Integer::Integer(){
    negative = false;
    part.push_back(0);
}
Integer::Integer(const long& n){
    negative = (n<0);
    part.push_back((negative)?-n:n);
}
Integer::Integer(const String& b, long lbase) : negative(false){
    Integer base(lbase);
    L_Iter(b,c){
        long v;
        if(*c>='a'&&*c<='z') // Go from visual char to actual value
            v = *c-('a'-10);
        else if(*c>='A'&&*c<='Z') // Same
            v = *c-('A'-10);
        else if(*c>='0'&&*c<='9') // Same
            v = *c-'0';
        else continue; // Erase useless characters
        operator*=(base);
        operator+=(v);
    }
    negative = (b[0]=='-');
}

Integer Integer::operator+(const Integer& other) const{
    Integer wtr(*this);
    return wtr += other;
}
Integer Integer::operator-(const Integer& other) const{
    Integer wtr(*this);
    return wtr -= other;
}
Integer Integer::operator+() const{
    return *this;
}
Integer Integer::operator-() const{
    Integer wtr(*this);
    wtr.negative = !wtr.negative;
    return wtr;
}
Integer Integer::operator*(const Integer& other) const{
    Integer wtr(*this);
    return wtr *= other;
}
Integer Integer::operator/(const Integer& other) const{
    Integer wtr(*this);
    return wtr /= other;
}
Integer Integer::operator%(const Integer& other) const{
    Integer wtr(*this);
    return wtr %= other;
}
Integer& Integer::operator++(){
    *this+=(long)1;
    return *this;
}
Integer Integer::operator++(int){
    Integer wtr = *this;
    *this+=(long)1;
    return wtr;
}
Integer& Integer::operator--(){
    *this-=(long)1;
    return *this;
}
Integer Integer::operator--(int){
    Integer wtr = *this;
    *this-=(long)1;
    return wtr;
}

bool Integer::operator==(const Integer& other) const{
    size_t maxp = std::max(part.size(),other.part.size());
    type zeroTest = 0, equalityTest = 0;

    for(size_t n=0;n<maxp;n++){
        if(gPart(n)==other.gPart(n)){
            equalityTest++;
            if(gPart(n)==0)
                zeroTest++;
        }
    }
    if((negative != other.negative) && (zeroTest != maxp))
        return false;
    else if(equalityTest == maxp)
        return true;
    else
        return false;
}
bool Integer::operator!=(const Integer& other) const{
    return !(*this==other);
}
bool Integer::operator>(const Integer& other) const{
    if(negative==other.negative){
        size_t maxp = std::max(part.size(),other.part.size());
        for(size_t n=maxp-1;n<=maxp;n--){
            if(gPart(n)>other.gPart(n))
                return !negative;
            else if(gPart(n)<other.gPart(n))
                return negative;
        }
        return false;
    }
    else return other.negative; // This is negative -> false, other is negative -> true
}
bool Integer::operator<(const Integer& other) const{
    if(negative==other.negative){
        size_t maxp = std::max(part.size(),other.part.size());
        for(size_t n=maxp-1;n<=maxp;n--){
            if(gPart(n)<other.gPart(n))
                return !negative;
            else if(gPart(n)>other.gPart(n))
                return negative;
        }
        return false;
    }
    else return negative; // This is negative -> true, b is negative -> false
}
bool Integer::operator>=(const Integer& other) const{
    return !(*this<other);
}
bool Integer::operator<=(const Integer& other) const{
    return !(*this>other);
}

Integer Integer::operator<<(const size_t& n) const{
    /*
    Integer wtr;
    for(size_t i=part.size();i!=(size_t)-1;i--){
        wtr.sPart(i,gPart(i)<<b);
        wtr.sPart(i+1,wtr.gPart(i+1)|(gPart(i)>>(32-b)));
    }
    return wtr;
    */
    Integer wtr(*this);
    return wtr <<= n;
}
Integer Integer::operator>>(const size_t& n) const{
    /*
    Integer wtr;
    for(size_t i=0;i<part.size();i++){
        wtr.sPart(i,gPart(i)>>b);
        if(i)wtr.sPart(i-1,wtr.gPart(i-1)|(gPart(i)<<(32-b)));
    }
    */
    Integer wtr(*this);
    return wtr >>= n;
}

Integer& Integer::operator+=(const Integer& other){
    if(negative != other.negative)
        (*this) -= -other;
    else{ // Same sign
        size_t maxp(std::max(part.size(),other.part.size()));
        for(size_t n(0);n<maxp;n++){
            type tmp(gPart(n));
            sPart(n,tmp+other.gPart(n));
            if(tmp>gPart(n)){
                int i=0;
                do{
                    i++;
                    sPart(n+i,gPart(n+i)+1);
                }while(gPart(n+i)==0);
            }
        }
        trim();
    }
    return *this;
}
Integer& Integer::operator-=(const Integer& other){
    if(negative != other.negative)
        (*this) += -other;
    else if((negative && (*this) > other) || (!negative && (*this) < other)) // Only subtract absolutely smaller number
        (*this) = -(other-(*this));
    else{ // Same sign
        size_t maxp(std::max(part.size(),other.part.size()));
        for(size_t n(0);n<maxp;n++){
            type tmp(gPart(n));
            sPart(n,tmp-other.gPart(n));
            if(tmp<gPart(n)){
                int i(0);
                do{
                    i++;
                    sPart(n+i,gPart(n+i)-1);
                }while(gPart(n+i)==(type)-1);
            }
        }
        trim();
    }
    return *this;
}
Integer& Integer::operator*=(const Integer& other){
    Integer x(abs()), y(other.abs());
    bool resNeg(negative!=other.negative);
    reset(); // Make this zero
    while(y>0){
        if(y.gPart(0)&1){
            (*this)+=x;
            y--;
        }
        else{
            y>>=1;
            x<<=1;
        }
    }
    negative = resNeg;
    trim();
    return *this;
}
Integer& Integer::operator/=(const Integer& other){
    if(other==0)
        throw Exception("Trying to divide by zero.");
    Integer a(abs()), b(other.abs());
    if(a < b)
        reset();
    else{
        Integer limDown, limUp(1);
        size_t n(0);
        bool resNeg(negative != other.negative);

        // Calculating first limits
        while(limUp*b<=a){
            limUp <<= 1;
            n++;
        }
        limDown = limUp >> 1;

        // Using dichotomy
        for(size_t i=0;i<n;i++){
            (*this) = (limDown + limUp) >> 1;
            if((*this)*b <= a)  limDown = (*this);
            else                limUp = (*this);
        }
        negative = resNeg;
        trim();
    }
    return *this;
}
Integer& Integer::operator%=(const Integer& other){
    if(other==0)
        throw Exception("Trying to mod by zero.");
    (*this) -= ((*this/other)*other);
    trim();
    return *this;
}
Integer& Integer::operator<<=(const size_t& n){
    for(size_t i(part.size());i!=(size_t)-1;i--){
        type tmp(gPart(i));
        sPart(i,tmp<<n);
        sPart(i+1,gPart(i+1)|(tmp>>(typebits-n)));
    }
    trim();
    return *this;
}
Integer& Integer::operator>>=(const size_t& n){
    for(size_t i(0);i<part.size();i++){
        type tmp(gPart(i));
        sPart(i,tmp>>n);
        if(i)sPart(i-1,gPart(i-1)|(tmp<<(typebits-n)));
    }
    trim();
    return *this;
}

Integer Integer::abs() const{
    Integer wtr(*this);
    wtr.negative = false;
    return wtr;
}
String Integer::toShortString() const{
    switch(part.size()){
        case 0:  return "0";
        case 1:  return ((negative)?"-":"")+ToString(part[0]);
        case 2:  return ((negative)?"-":"")+ToString(part.back())+"*(2^32)";
        default: return ((negative)?"-":"")+ToString(part.back())+"*(2^32^"+ToString(part.size()-1)+")";
    }
}
String Integer::toString(long lbase) const{
    Integer base = lbase, n;
    type buff;
    String wtr = "";

    switch(lbase){
        case 2:
            for(size_t i=part.size()-1;i!=(size_t)-1;i--)
                wtr += Binary(gPart(i));
            while(wtr[0]=='0' && wtr.size()>1)
                wtr.erase(0,1);
            break;
        case 16:
            for(size_t i=part.size()-1;i!=(size_t)-1;i--)
                wtr += Hexadecimal(gPart(i));
            while(wtr[0]=='0' && wtr.size()>1)
                wtr.erase(0,1);
            break;
        default:
            n = abs();
            do{
                buff = (n%base).gPart(0);
                wtr = (char)((buff<10) ? buff+'0' : buff+'W') + wtr;
                n /= base;
            }while(n.part.size()!=0);
            break;
    }
    if(negative) wtr = '-' + wtr;
    return wtr;
}

Integer::type Integer::gPart(size_t position) const{
    if(position<part.size())
        return part[position];
    else
        return 0;
}
void Integer::sPart(size_t position, type p){
    while(part.size()<=position)
        part.push_back(0);
    part[position] = p;
}
void Integer::reset(){
    negative = false;
    part.clear();
}
void Integer::trim(){
    while(part.size() && part.back()==0)
        part.pop_back();
}

Integer Integer::lcd(const Integer& a, const Integer& b){
    Integer c = a, d = b;
    while(c!=d){
        if(c<d)
            c+=a;
        else
            d+=b;
    }
    return c;
}
Integer Integer::gcd(Integer a, Integer b){
    if(a<b)
        L::swap(a,b);
    if(b!=0){
        Integer c;
        while((b!=1)&&(a%b!=0)){
            c = a%b;
            a = b;
            b = c;
        }
        return b;
    }
    else return a;
}
Integer Integer::pow(const Integer& a, Integer b){
    Integer wtr = 1;
    while(b--!=0) wtr *= a;
    return wtr;
}

std::ostream& L::operator<<(std::ostream &stream, const Integer& v){
    if(v.part.size()){
        if(v.negative)
            stream << '-';
        stream << ToString(v.part.back());
        if(v.part.size()>1){
            stream << "*(2^32";
            if(v.part.size()==2)
                stream << ')';
            else
                stream << '^' << ToString(v.part.size()-1) << ')';
        }
    }
    else stream << '0';
    return stream;
}


