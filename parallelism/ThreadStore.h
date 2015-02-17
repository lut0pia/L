#ifndef DEF_L_ThreadStore
#define DEF_L_ThreadStore

#include "Thread.h"
#include "../containers/Ref.h"
#include "Mutex.h"
#include "../system.h"

namespace L{
    class ThreadStore{
        private:
            Vector<Ref<Thread> > threads;
            Mutex mutex;
            static Dynamic::Var garbageCollector(Thread*);

        public:
            ThreadStore();
            L_NoCopy(ThreadStore)
            ~ThreadStore();

            void attach(const Ref<Thread>&);
    };
}
#endif




