#ifndef DEF_L_Dynamic_Type
#define DEF_L_Dynamic_Type

#include <iostream>
#include <typeinfo>
#include "../macros.h"
#include "../stl/String.h"

namespace L {
  namespace Dynamic {
    // Structure to keep basic functions for a type
    typedef struct {
      void* (*cpy)(void*);
      void (*del)(void*);

      String(*name)();
      void (*out)(std::ostream&,void*);
      int (*cmp)(void*,void*);

      bool (*hasname)();
      bool (*hasout)();
      bool (*hascmp)();
    } TypeDescription;

    template <class T>
    class Type {
      private:
        static const TypeDescription td;

      public:
        static inline const TypeDescription* description() {return &td;}

        static void* cpy(void* p) {return new T(*(T*)p);}
        static void del(void* p) {delete(T*)p;}

        static String name() {
          // "static L::String L::Dynamic::Type<T>::name() [with T = XXX]"
#if defined _MSC_VER
          String wtr(__FUNCDNAME__); // Relies on GCC
#else
          String wtr(__PRETTY_FUNCTION__); // Relies on GCC
#endif
          wtr.trimRight(); // ]
          return wtr.substr(55);
        }
        static void out(std::ostream& s, void* p) {s << "N/A";}
        static int cmp(void* a, void* b) {return 0;}

        static bool hasname() {return true;}
        static bool hasout() {return false;}
        static bool hascmp() {return false;}
    };

    // Instantiate structures
    template <class T> const TypeDescription Type<T>::td = {cpy,del,
                                                            name,out,cmp,
                                                            hasname,hasout,hascmp
                                                           };

    // Specify void doesn't construct nor destruct
    template <> inline void* Type<void>::cpy(void* p) {return NULL;}
    template <> inline void Type<void>::del(void* p) {}

#define L_TYPE_NAMED(T)   template <> inline String Type< T >::name(){return #T;} \
  template <> inline bool Type< T >::hasname(){return true;}

#define L_TYPE_OUTABLE(T) template <> inline void Type< T >::out(std::ostream& s, void* p){s << (*(T*)p);} \
  template <> inline bool Type< T >::hasout(){return true;}

#define L_TYPE_CMPABLE(T) template <> inline int Type< T >::cmp(void* a, void* b){\
    if((*(T*)a)<(*(T*)b))       return -1;\
    else if((*(T*)a)==(*(T*)b)) return 0;\
    else                        return 1;} \
  template <> inline bool Type< T >::hascmp(){return true;}

#define L_TYPE_PRIMITIVE(T) L_TYPE_NAMED(T) L_TYPE_OUTABLE(T) L_TYPE_CMPABLE(T)

    L_TYPE_PRIMITIVE(bool)
    L_TYPE_PRIMITIVE(unsigned char)
    L_TYPE_PRIMITIVE(char)
    L_TYPE_PRIMITIVE(unsigned short)
    L_TYPE_PRIMITIVE(short)
    L_TYPE_PRIMITIVE(uint)
    L_TYPE_PRIMITIVE(int)
    L_TYPE_PRIMITIVE(unsigned long)
    L_TYPE_PRIMITIVE(long)
    L_TYPE_PRIMITIVE(unsigned long long)
    L_TYPE_PRIMITIVE(long long)
    L_TYPE_PRIMITIVE(float)
    L_TYPE_PRIMITIVE(double)
    L_TYPE_PRIMITIVE(long double)
    L_TYPE_PRIMITIVE(String)
  }
}

#endif



