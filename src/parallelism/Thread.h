#pragma once

#if defined L_WINDOWS
#include <windows.h>
#elif defined L_UNIX
#include <pthread.h>
#endif

#include "../dynamic/Variable.h"
#include "../macros.h"

namespace L {
  class Thread {
    L_NOCOPY(Thread)
  public:
    typedef Var(*Function)(Thread*);
  private:
    Function _function;
    Var _arg,_result;
    bool _running : 1,_waitedFor : 1;
#if defined L_WINDOWS
    DWORD _threadId;
    HANDLE _threadHandle;
#elif defined L_UNIX
    pthread_t _thread;
#endif
    static void proxy(Thread*);

  public:
    inline Thread() : _running(false),_waitedFor(false) {}
    inline Thread(Function f,const Var& a = Var()) : _running(true),_waitedFor(false) { start(f,a); }
    inline ~Thread() { terminate(); }

    void start(Function,const Var& = Var());
    void wait(int maxMilliseconds = -1);
    void terminate();
    void affinity(uint32_t mask);

    inline const Var& arg() const { return _arg; }
    inline const Var& result() const { return _result; }
    inline bool running() const { return _running; }
    inline bool waitedFor() const { return _waitedFor; }

    friend class ThreadPool;
  };
}
