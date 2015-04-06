#ifndef DEF_L_YStream
#define DEF_L_YStream

#include <iostream>
#include "../macros.h"

namespace L {
  class YStream {
    public:
      Set<std::ostream*> streams;

      template <class T>
      YStream& operator <<(const T& value) {
        L_Iter(streams,it)
        *(*it) << value;
        return *this;
      }

  };
}

#endif
