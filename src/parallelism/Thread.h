#ifndef DEF_L_Thread
#define DEF_L_Thread

#if defined L_WINDOWS
#include <windows.h>
#elif defined L_UNIX
#include <pthread.h>
#endif

#include <cstdio>
#include <cstdlib>
#include "../dynamic/Variable.h"
#include "../macros.h"

namespace L {
  class Thread {
      typedef Var(*Function)(Thread*);
    private:
      Function function;
      Var arg, result;
      bool running, waitedFor;
#if defined L_WINDOWS
      DWORD threadId;
      HANDLE threadHandle;
#elif defined L_UNIX
      pthread_t thread;
#endif
      static void proxy(Thread*);

    public:
      Thread();
      Thread(Function,const Var& = Var());
      L_NoCopy(Thread)
      ~Thread();

      void start(Function,const Var& = Var());
      void waitForEnd(int maxMilliseconds = -1);
      void terminate();

      inline const Var& gArg() const {return arg;}
      inline const Var& gResult() const {return result;}
      inline bool isRunning() const {return running;}
      inline bool isWaitedFor() const {return waitedFor;}

      friend class ThreadPool;
  };
}
#endif




