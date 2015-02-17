#ifndef DEF_L_Observable
#define DEF_L_Observable

#include "../macros.h"
#include "../stl.h"

#include "Observer.h"

namespace L{
    class Observable{
        private:
            Set<Observer*> observers;

        public:
            void addObserver(Observer* o){
                o->observed.insert(this);
                observers.insert(o);
            }
            void removeObserver(Observer* o){
                o->observed.erase(this);
                observers.erase(o);
            }
            void notifyObservers(int msg = 0){
                L_Iter(observers,it)
                    (*it)->notification(this,msg);
            }
    };
}

#endif


