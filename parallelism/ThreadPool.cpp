#include "ThreadPool.h"

using namespace L;

Dynamic::Var ThreadPool::worker(Thread* thread){
    ThreadPool& tp(*thread->gArg().as<ThreadPool*>());
    while(!tp.killing){
        tp.todoSem.wait();
        tp.tasksMutex.lock();
        if(!tp.tasks.empty()){
            TaskArg ta(tp.tasks.front());
            tp.tasks.pop_front();
            tp.tasksMutex.unlock();
            Dynamic::Var res(ta.task(ta.arg));
            tp.resultsMutex.lock();
            tp.results.push_back(res);
            tp.resultsMutex.unlock();
            tp.doneSem.post();
        }
        else tp.tasksMutex.unlock();
    }
    return Dynamic::Var();
}

ThreadPool::ThreadPool(size_t n) : threads(n), todoSem(0), doneSem(0), killing(false), awaiting(0){
    for(size_t i(0);i<n;i++)
        threads[i] = new Thread(worker,this);
}
ThreadPool::~ThreadPool(){
    tasksMutex.lock();
    tasks.clear();
    tasksMutex.unlock();
    killing = true;
    todoSem.post(threads.size());
    L_Iter(threads,it)
        (*it)->waitForEnd();
}

void ThreadPool::push(Task task, const Dynamic::Var& arg){
    tasksMutex.lock();
    tasks.push_back({task,arg});
    awaiting++;
    todoSem.post();
    tasksMutex.unlock();
}
Dynamic::Var ThreadPool::get(){
    doneSem.wait();
    awaiting--;
    resultsMutex.lock();
    Dynamic::Var wtr(results.front());
    results.pop_front();
    resultsMutex.unlock();
    return wtr;
}
