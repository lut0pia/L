#include "ThreadStore.h"

using namespace L;

Dynamic::Var ThreadStore::garbageCollector(Thread* thread){
    ThreadStore& ts(*thread->gArg().as<ThreadStore*>());
    while(!thread->isWaitedFor()){
        size_t i(0);
        ts.mutex.lock();
        while(i<ts.threads.size()){
            if(!ts.threads[i]->isRunning())
                ts.threads.erase(ts.threads.begin()+i);
            else i++;
        }
        ts.mutex.unlock();
        System::sleep(50);
    }
    return Dynamic::Var();
}
ThreadStore::ThreadStore(){
    attach(new Thread(garbageCollector,this));
}
ThreadStore::~ThreadStore(){
    mutex.lock();
    L_Iter(threads,it)
        (*it)->waitForEnd();
    mutex.unlock();
}

void ThreadStore::attach(const Ref<Thread>& thread){
    mutex.lock();
    threads.push_back(thread);
    mutex.unlock();
}

