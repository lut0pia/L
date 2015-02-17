#ifndef DEF_L_MTF
#define DEF_L_MTF

#include "../macros.h"
#include "../stl/Vector.h"

namespace L{
    class MTF{
        public:
            static Vector<byte> encode(const Vector<byte>&);
            static Vector<byte> decode(const Vector<byte>&);
    };
}

#endif






