#include "Ref.h"

#include "../stl/Map.h"
#include "../parallelism/Mutex.h"

using namespace L;

Map<void*,size_t> refCount;
Mutex mutex;

size_t L::ReferencedObjects(){
    size_t wtr;
    mutex.lock();
    wtr = refCount.size();
    mutex.unlock();
    return wtr;
}
void L::increment(void* p){
    mutex.lock();
    refCount[p]++;
    //cout << p << " : " << refCount[p] << endl;
    mutex.unlock();
}
bool L::decrement(void* p){
    mutex.lock();
    refCount[p]--;
    //cout << p << " : " << refCount[p] << endl;
    if(refCount[p]==0){
        refCount.erase(p);
        //if(!refCount.size())
            //cout << "Last reference deleted" << endl;
        //else
            //cout << refCount.size() << " references left" << endl;
        mutex.unlock();
        return true;
    }
    else{
        mutex.unlock();
        return false;
    }
}
