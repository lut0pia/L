#ifndef DEF_L_String
#define DEF_L_String

#include "../containers/Array.h"
#include "../streams/Stream.h"

namespace L {
  namespace Pending {
    class String: public Array<char> {
      public:
        inline String() {}
        String(const char*);
        inline String operator+(const String& other) {
          String wtr(*this);
          return wtr += other;
        }
        inline String& operator+=(const String& other) {
          pop();
          Array<char>::operator+=(other);
          return *this;
        }
    };
  }
  inline Stream& operator<<(Stream &s, const Pending::String& str) {
    return s << &str[0];
  }
  inline Stream& operator<(Stream &s, const Pending::String& str) {
    s < str.size();
    s.write(&str[0],str.size());
    return s;
  }
}

#endif

