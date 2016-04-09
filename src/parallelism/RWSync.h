#ifndef DEF_L_RWSync
#define DEF_L_RWSync

#include "Mutex.h"

namespace L{
    class RWSync{
        private:
            Mutex writing, countAccess;
            size_t readers;
        public:
            RWSync();
            L_NoCopy(RWSync)
            void startRead();
            void stopRead();
            void startWrite();
            void stopWrite();
    };
}

#endif





