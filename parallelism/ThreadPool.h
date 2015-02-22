#ifndef DEF_L_ThreadPool
#define DEF_L_ThreadPool

#include "Thread.h"
#include "../containers/Ref.h"
#include "Mutex.h"
#include "Semaphore.h"

namespace L{
    class ThreadPool{
        typedef Dynamic::Var (*Task)(const Dynamic::Var&);
        typedef struct{Task task; Dynamic::Var arg;} TaskArg;
        private:
            Vector<Ref<Thread> > threads;
            List<TaskArg> tasks;
            List<Dynamic::Var> results;
            Mutex tasksMutex, resultsMutex;
            Semaphore todoSem, doneSem;
            bool killing;
            size_t awaiting;

            static Dynamic::Var worker(Thread*);

        public:
            ThreadPool(size_t);
            L_NoCopy(ThreadPool)
            ~ThreadPool();

            inline bool waiting(){return awaiting;}
            void push(Task, const Dynamic::Var& = Dynamic::Var());
            Dynamic::Var get();
    };
}
#endif




