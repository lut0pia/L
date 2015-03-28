#ifndef DEF_L_Semaphore
#define DEF_L_Semaphore

#if defined L_WINDOWS
#include <windows.h>
#elif defined L_UNIX
#include <semaphore.h>
#endif

#include "../types.h"
#include "../macros.h"

namespace L {
  class Semaphore {
    private:
#if defined L_WINDOWS
      HANDLE sem;
#elif defined L_UNIX
      sem_t sem;
#endif
    public:
      Semaphore(uint);
      L_NoCopy(Semaphore)
      ~Semaphore();
      void wait();
      void wait(uint);
      void post();
      void post(uint);
  };
}

#endif




