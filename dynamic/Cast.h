#ifndef DEF_L_Dynamic_Cast
#define DEF_L_Dynamic_Cast

#include "Type.h"
#include "../containers/Map.h"
#include "../types.h"

namespace L {
  typedef void (*CastFct)(void*&);
  typedef union {
    struct {const TypeDescription *from, *to;} types;
    ullong id;
  } CastDescription;
  class Cast {
    public:
      static Map<ullong,CastFct> casts;

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
        cd.types.from = from;
        cd.types.to = to;
        const KeyValue<ullong,CastFct>* it(casts.find(cd.id));
        return (it)?it->value():NULL;
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

#endif



