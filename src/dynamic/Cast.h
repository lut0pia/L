#ifndef DEF_L_Dynamic_Cast
#define DEF_L_Dynamic_Cast

#include "Type.h"
#include "../containers/Map.h"
#include "../types.h"

namespace L {
  class Variable;
  typedef void (*CastFct)(const Variable& a,Variable& b);
  class Cast {
    private:
      typedef union {
        struct { const TypeDescription *from,*to; } types;
        uint64_t id;
      } CastDescription;
      static Map<uint64_t,CastFct> casts;

      template <class A, class B>
      static void func(const Variable& a,Variable& b);

    public:
      static void init();
      static CastFct get(const TypeDescription* from, const TypeDescription* to) {
        CastDescription cd;
        cd.types.from = from;
        cd.types.to = to;
        const KeyValue<uint64_t,CastFct>* it(casts.find(cd.id));
        return (it)?it->value():nullptr;
      }

      template <class A, class B>
      static inline void declare() {
        CastDescription cd;
        cd.types.from = Type<A>::description();
        cd.types.to = Type<B>::description();
          declare(cd,func<A,B>);
      }
      static inline void declare(CastDescription cd, CastFct cf) {
        if(cd.types.from!=cd.types.to) // Don't declare casts for the same type
          casts[cd.id] = cf;
      }
  };
}

#include "Variable.h"

namespace L{
  template <class A,class B>
  static void Cast::func(const Variable& a,Variable& b) {
    b = (B)a.as<A>();
  }
}

#endif



