#ifndef DEF_L_BWT
#define DEF_L_BWT

#include "../types.h"
#include "../stl/Vector.h"

namespace L{
    class BWT{
        public:
            static Vector<byte> encode(const Vector<byte>&, size_t& index);
            static Vector<byte> decode(const Vector<byte>&, size_t index);
    };
}

#endif






