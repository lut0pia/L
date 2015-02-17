#ifndef DEF_L_Singleton
#define DEF_L_Singleton

#include "../containers/Ref.h"
#include "../macros.h"

namespace L{
    template <class T>
    class Singleton{
        private:
            static Ref<T> instance;

        public:
            static T& get(){
                if(instance.null())
                    instance = new T();
                return *instance;
            }
            static bool exists(){
                return !instance.null();
            }
            static void unget(){
                instance.clear();
            }
    };
    template <class T> Ref<T> Singleton<T>::instance;
}

#endif


