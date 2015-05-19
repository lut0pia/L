#ifndef DEF_L_MTF
#define DEF_L_MTF

#include "../macros.h"
#include "../containers/Array.h"

namespace L{
    class MTF{
        public:
            static Array<byte> encode(const Array<byte>&);
            static Array<byte> decode(const Array<byte>&);
    };
}

#endif






