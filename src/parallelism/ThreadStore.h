#ifndef DEF_L_ThreadStore
#define DEF_L_ThreadStore

#include "Thread.h"
#include "../containers/Ref.h"
#include "Mutex.h"

namespace L {
  class ThreadStore {
    private:
      Array<Ref<Thread> > threads;
      Mutex mutex;
      static Var garbageCollector(Thread*);

    public:
      ThreadStore();
      L_NoCopy(ThreadStore)
      ~ThreadStore();

      void attach(const Ref<Thread>&);
  };
}
#endif




