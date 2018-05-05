#pragma once

#include "../container/Table.h"
#include "../stream/serial.h"
#include "../text/Symbol.h"
#include "../hash.h"

namespace L {
  typedef void(*Cast)(void*,const void*);
  // Structure to keep basic functions for a type
  struct TypeDescription{
    // Mandatory
    Symbol name;
    int size;

    void(*ctr)(void*);
    void* (*ctrnew)();
    void* (*cpy)(void*);
    void(*cpyto)(void*,const void*);
    void(*assign)(void*,const void*);
    void(*dtr)(void*);
    void(*del)(void*);
    void(*print)(Stream&, const void*);
    void(*out)(Stream&, const void*);
    void(*in)(Stream&, void*);
    uint32_t(*hash)(const void*);

    // Optional
    void(*add)(void*,const void*);
    void(*sub)(void*,const void*);
    void(*mul)(void*,const void*);
    void(*div)(void*,const void*);
    void(*mod)(void*,const void*);

    int(*cmp)(const void*,const void*);

    Table<const TypeDescription*, Cast> casts;
  };

  template <typename T>
  Symbol type_name() {
    static Symbol wtr;
    if(!wtr) {
#if defined _MSC_VER
      // "class L::Symbol __cdecl L::type_name<int>(void)"
      // "class L::Symbol __cdecl L::type_name<class L::String>(void)"
      char funcsig[] = __FUNCSIG__;
      const uintptr_t start(sizeof("class L::Symbol __cdecl L::type_name<")-1);
      const uintptr_t end(strlen(funcsig)-sizeof(">(void)")+1);
      funcsig[end] = '\0';
      char* name(funcsig+start);
#else
      // "static L::TypeDescription L::Type<T>::makeDesc() [with T = XXX]"
      char tmp[256];
      strcpy(tmp, __PRETTY_FUNCTION__+59);
      tmp[strlen(tmp)-1] = '\0';
      char* name(tmp);
#endif
      const char* subs[] ={ "class ","struct ", " " };
      for(const char* sub : subs) {
        while(char* found = strstr(name, sub)) {
          size_t sub_len(strlen(sub));
          memmove(found, found+sub_len, strlen(name)-(found-name)+sub_len+1);
        }
      }
      wtr = name;
    }
    return wtr;
  }

  extern Table<Symbol, const TypeDescription*> types;
  template <class T>
  class Type {
  private:
    static TypeDescription td;
    static TypeDescription makeDesc() {
      TypeDescription wtr = {
        type_name<T>(),sizeof(T),
        ctr,ctrnew,cpy,cpyto,assign,dtr,del,print,out,in,Type<T>::hash,0
      };
      types[wtr.name] = &td;
      return wtr;
    }
    static void ctr(void* p) { ::new(p)T(); }
    static void* ctrnew() { return new T; }
    static void* cpy(void* p) { return new T(*(T*)p); }
    static void cpyto(void* dst,const void* src) { ::new((T*)dst) T(*(const T*)src); }
    static void assign(void* dst,const void* src) { *(T*)dst = *(const T*)src; }
    static void dtr(void* p) { ((T*)p)->~T(); }
    static void del(void* p) { delete(T*)p; }
    static void print(Stream& s,const void* p) { s << (*(const T*)p); }
    static void out(Stream& s, const void* p) { s < (*(const T*)p); }
    static void in(Stream& s, void* p) { s > (*(T*)p); }
    static uint32_t hash(const void* p) { return L::hash(*(const T*)p); }

  public:
    static inline const TypeDescription* description() { return &td; }
    static inline const char* name() { return td.name; }

    // Casts
    static inline void addcast(const TypeDescription* otd, Cast cast) { td.casts[otd] = cast; }
    template <class R> static inline void addcast(Cast cast){ addcast(Type<R>::description(),cast); }
    template <class R> static inline void addcast(){ addcast<R>([](void* dst,const void* src){new(dst)R(*(T*)src); }); }

    // Operator setters
    static inline void useadd(void(*add)(void*,const void*)){ td.add = add; }
    static inline void usesub(void(*sub)(void*,const void*)){ td.sub = sub; }
    static inline void usemul(void(*mul)(void*,const void*)){ td.mul = mul; }
    static inline void usediv(void(*div)(void*,const void*)){ td.div = div; }
    static inline void usemod(void(*mod)(void*,const void*)){ td.mod = mod; }
    static inline void usecmp(int(*cmp)(const void*,const void*)){ td.cmp = cmp; }

    // Operator default setters
    template <class dummy = void> static inline void canall(){ cancmp<>(); canmath<>(); canmod<>(); }
    template <class dummy = void> static inline void canmath(){ canadd<>(); cansub<>(); canmul<>(); candiv<>(); }
    template <class dummy = void> static inline void canadd(){ useadd([](void* a,const void* b) { *((T*)a) += *((T*)b); }); }
    template <class dummy = void> static inline void cansub(){ usesub([](void* a,const void* b) { *((T*)a) -= *((T*)b); }); }
    template <class dummy = void> static inline void canmul(){ usemul([](void* a,const void* b) { *((T*)a) *= *((T*)b); }); }
    template <class dummy = void> static inline void candiv(){ usediv([](void* a,const void* b) { *((T*)a) /= *((T*)b); }); }
    template <class dummy = void> static inline void canmod(){ usemod([](void* a,const void* b) { *((T*)a) %= *((T*)b); }); }
    template <class dummy = void> static inline void cancmp(){
      usecmp([](const void* a,const void* b)->int {
        if((*(T*)a)<(*(T*)b))       return -1;
        else if((*(T*)b)<(*(T*)a))  return 1;
        else                        return 0;
      });
    }
  };
  template <>
  inline TypeDescription Type<void>::makeDesc() {
    TypeDescription wtr = {
      "void",0,
      [](void*) {},
      []() -> void* { return nullptr; },
      [](void*) -> void* { return nullptr; },
      [](void*, const void*) {},
      [](void*, const void*) {},
      [](void*) {},
      [](void*) {},
      [](Stream&, const void*) {},
      [](Stream&, const void*) {},
      [](Stream&, void*) {},
      [](const void*) -> uint32_t { return 0; }
    };
    return wtr;
  }

  // Instantiate structures
  template <class T> TypeDescription Type<T>::td(Type<T>::makeDesc());

  void TypeInit();
}
