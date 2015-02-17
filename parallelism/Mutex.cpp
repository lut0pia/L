#include "Mutex.h"

using namespace L;

Mutex::Mutex(){
    #if defined L_WINDOWS
        mutex = CreateMutex(NULL,FALSE,NULL);
    #elif defined L_UNIX
        pthread_mutex_init(&mutex, NULL);
    #endif
}
Mutex::~Mutex(){
    #if defined L_WINDOWS
        CloseHandle(mutex);
    #elif defined L_UNIX
        pthread_mutex_destroy(&mutex);
    #endif
}
void Mutex::lock(){
    #if defined L_WINDOWS
        WaitForSingleObject(mutex,INFINITE);
    #elif defined L_UNIX
        pthread_mutex_lock(&mutex);
    #endif
}
void Mutex::unlock(){
    #if defined L_WINDOWS
        ReleaseMutex(mutex);
    #elif defined L_UNIX
        pthread_mutex_unlock(&mutex);
    #endif
}
