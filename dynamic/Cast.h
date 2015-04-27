#ifndef DEF_L_Dynamic_Cast
#define DEF_L_Dynamic_Cast

#include "../stl.h"
#include "Type.h"

namespace L {
  namespace Dynamic {
    typedef void (*CastFct)(void*&);
    typedef union {
      struct {const TypeDescription *from, *to;} types;
      unsigned long long id;
    } CastDescription;
    class Cast {
      public:
        static Map<unsigned long long,CastFct> casts;

        template <class A, class B>
        static void func(void*& p) {
          B* wtr(new B((B)*(A*)p));
          delete(A*)p;
          p = wtr;
        }

      public:
        static void init();
        static CastFct get(const TypeDescription* from, const TypeDescription* to) {
          CastDescription cd;
          Map<unsigned long long,CastFct>::iterator it;
          cd.types.from = from;
          cd.types.to = to;
          return ((it = casts.find(cd.id))!=casts.end())?it->second:NULL;
        }

        template <class A, class B>
        static void declare() {
          CastDescription cd;
          cd.types.from = Type<A>::description();
          cd.types.to = Type<B>::description();
          declare(cd,func<A,B>);
        }
        static void declare(CastDescription cd, CastFct cf) {
          casts[cd.id] = cf;
        }
    };
  }
}

#endif



