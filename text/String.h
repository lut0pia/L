#ifndef DEF_L_String
#define DEF_L_String

#include "../containers/Array.h"

namespace L {
  namespace Pending {
    class String: private Array<char> {
      public:
        inline String() {}
        String(const char*);
        String(const String&, size_t start, size_t length = -1);

        inline String operator+(const String& other) {
          String wtr(*this);
          return wtr += other;
        }
        inline String& operator+=(const String& other) {
          pop();
          Array<char>::operator+=(other);
          return *this;
        }

        String substr(size_t pos, size_t length = -1) const;

        inline size_t size() const {return Array<char>::size()-1;} // Subtract one because of '\0'
        inline operator const char*() const {return &Array<char>::operator[](0);}
    };
  }
}

#endif

