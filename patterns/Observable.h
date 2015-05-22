#ifndef DEF_L_Observable
#define DEF_L_Observable

#include "../macros.h"
#include "../stl.h"

#include "Observer.h"

namespace L {
  class Observable {
    private:
      Set<Observer*> _observers;

    public:
      void addObserver(Observer* o) {
        o->observed.insert(this);
        _observers.insert(o);
      }
      void removeObserver(Observer* o) {
        o->observed.erase(this);
        _observers.erase(o);
      }
      void notifyObservers(int msg = 0) {
        for(int i(0); i<_observers.size(); i++)
          _observers[i]->notification(this,msg);
      }
  };
}

#endif


