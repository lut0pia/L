#include "RWSync.h"

using namespace L;

RWSync::RWSync(){
    readers = 0;
}
void RWSync::startRead(){
    countAccess.lock();
    if(readers++==0)
        writing.lock();
    countAccess.unlock();
}
void RWSync::stopRead(){
    countAccess.lock();
    if(--readers==0)
        writing.unlock();
    countAccess.unlock();
}
void RWSync::startWrite(){
    writing.lock();
}
void RWSync::stopWrite(){
    writing.unlock();
}
