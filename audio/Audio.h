#ifndef DEF_L_Audio
#define DEF_L_Audio

#include "../stl/String.h"

namespace L{
    namespace Audio{
        void playSound(const String& filename, size_t flags = 0);
    }
}

#endif


