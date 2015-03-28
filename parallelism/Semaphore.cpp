#include "Semaphore.h"

using namespace L;

Semaphore::Semaphore(uint v){
    #if defined L_WINDOWS
        sem = CreateSemaphore(NULL,v,MAXLONG,NULL);
    #elif defined L_UNIX
        sem_init(&sem, 0, v);
    #endif
}
Semaphore::~Semaphore(){
    #if defined L_WINDOWS
        CloseHandle(sem);
    #elif defined L_UNIX
        sem_destroy(&sem);
    #endif
}
void Semaphore::wait(){
    #if defined L_WINDOWS
        WaitForSingleObject(sem,INFINITE);
    #elif defined L_UNIX
        sem_wait(&sem);
    #endif
}
void Semaphore::wait(uint n){
    while(n--)
        wait();
}
void Semaphore::post(){
    #if defined L_WINDOWS
        ReleaseSemaphore(sem,1,NULL);
    #elif defined L_UNIX
        sem_post(&sem);
    #endif
}
void Semaphore::post(uint n){
    while(n--)
        post();
}
