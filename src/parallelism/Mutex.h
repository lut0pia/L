#pragma once

#if defined L_WINDOWS
#include <windows.h>
#elif defined L_UNIX
#include <pthread.h>
#endif

#include "../macros.h"

namespace L {
  class Mutex {
    private:
#if defined L_WINDOWS
      HANDLE mutex;
#elif defined L_UNIX
      pthread_mutex_t mutex;
#endif
    public:
      Mutex();
      L_NOCOPY(Mutex)
      ~Mutex();
      void lock();
      void unlock();
  };
}
