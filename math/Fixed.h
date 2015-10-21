#ifndef DEF_L_Fixed
#define DEF_L_Fixed

#include "../streams/Stream.h"
#include "../types.h"
#include "../tmp.h"

namespace L {
  class Fixed {
    private:
      int _raw;
    public:
      static const int bits = sizeof(int)*8;
      static const int halfbits = bits/2;
      static const int mul = static_pow<2,halfbits>::value;

      inline Fixed() {}
      inline Fixed(int i) : _raw(i<<halfbits) {}
      Fixed(float);
      Fixed(double);

      template <class T> inline Fixed operator+(const T& other) {Fixed wtr(*this); return wtr+=other;}
      template <class T> inline Fixed operator-(const T& other) {Fixed wtr(*this); return wtr-=other;}
      template <class T> inline Fixed operator*(const T& other) {Fixed wtr(*this); return wtr*=other;}
      template <class T> inline Fixed operator/(const T& other) {Fixed wtr(*this); return wtr/=other;}
      template <class T> inline Fixed operator%(const T& other) {Fixed wtr(*this); return wtr%=other;}
      inline Fixed operator-() {Fixed wtr; wtr._raw = -_raw; return wtr;}

      inline Fixed& operator+=(const Fixed& other) {_raw+=other._raw; return *this;}
      inline Fixed& operator-=(const Fixed& other) {_raw-=other._raw; return *this;}
      inline Fixed& operator*=(const Fixed& other) {
        /*_asm {
          mov ebx, this
          mov edx, other
          mov eax, dword ptr [ebx] // Get this value
          imul dword ptr [edx] // Multiply eax with other value
          mov ax,dx
          rol eax,16 // Swap words
          mov dword ptr [ebx], eax
        }*/
        _raw = ((llong)_raw*(llong)other._raw)>>halfbits;
        return *this;
      }
      inline Fixed& operator/=(const Fixed& other) {
        _asm {
          mov ebx, this
          mov eax, dword ptr [ebx] // Get this value
          mov ecx, other
          mov ecx, dword ptr [ecx] // Get other value
          cdq // Extend eax to edx:eax
          idiv ecx // Divide edx:eax by ecx
          mov word ptr[ebx+2], ax // Move integral part
          xor eax, eax // Clear eax for next idiv
          rol edx, 16 // Swap words
          movsx edx, dx // Extend to dword
          idiv ecx // Divide remainder
          add dword ptr[ebx], eax // Move fractional part
        }
        return *this;
      }
      inline Fixed& operator%=(const Fixed& other) {_raw %= other._raw; return *this;}
      template <class T> inline Fixed& operator+=(const T& other) {_raw += (int)(other*0x10000); return *this;}
      template <class T> inline Fixed& operator-=(const T& other) {_raw -= (int)(other*0x10000); return *this;}
      template <class T> inline Fixed& operator*=(const T& other) {_raw *= other; return *this;}
      template <class T> inline Fixed& operator/=(const T& other) {_raw /= other; return *this;}

      inline int raw() const {return _raw;}

      inline operator int(){return _raw>>16;}
      inline operator float(){return (float)_raw/mul;}
      inline operator double(){return (double)_raw/mul;}
  };
  Stream& operator<<(Stream&, const Fixed&);
}

#endif




