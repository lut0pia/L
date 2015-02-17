#ifndef DEF_L_Semaphore
#define DEF_L_Semaphore

#include "../macros.h"

#if defined L_WINDOWS
    #include <windows.h>
#elif defined L_UNIX
    #include <semaphore.h>
#endif

namespace L{
    class Semaphore{
        private:
            #if defined L_WINDOWS
                HANDLE sem;
            #elif defined L_UNIX
                sem_t sem;
            #endif
        public:
            Semaphore(size_t);
            L_NoCopy(Semaphore)
            ~Semaphore();
            void wait();
            void wait(size_t);
            void post();
            void post(size_t);
    };
}

#endif




