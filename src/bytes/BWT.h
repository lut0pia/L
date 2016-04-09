#ifndef DEF_L_BWT
#define DEF_L_BWT

#include "../types.h"
#include "../containers/Array.h"

namespace L {
  class BWT {
    public:
      static Array<byte> encode(const Array<byte>&, size_t& index);
      static Array<byte> decode(const Array<byte>&, size_t index);
  };
}

#endif






