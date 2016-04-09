#ifndef DEF_L_Singleton
#define DEF_L_Singleton

#include "../macros.h"

namespace L {
  template <class T>
  class Singleton {
    private:
      static T* instance;

    public:
      static T& get() {
        if(!instance)
          instance = new T();
        return *instance;
      }
      static bool exists() {
        return instance;
      }
      static void unget() {
        if(instance) {
          delete instance;
          instance = NULL;
        }
      }
  };
  template <class T> T* Singleton<T>::instance;
}

#endif


