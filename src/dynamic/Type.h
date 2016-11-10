#pragma once

#include "../containers/Table.h"
#include "../streams/Stream.h"
#include "../types.h"
#include "../hash.h"

namespace L {
  typedef void(*Cast)(void*,const void*);
  // Structure to keep basic functions for a type
  struct TypeDescription{
    // Mandatory
    char name[256];
    int size;

    void* (*cpy)(void*);
    void(*cpyto)(void*,const void*);
    void(*assign)(void*,const void*);
    void(*dtr)(void*);
    void(*del)(void*);
    void(*out)(Stream&,const void*);
    uint32_t(*hash)(const void*);

    // Optional
    void(*add)(void*,const void*);
    void(*sub)(void*,const void*);
    void(*mul)(void*,const void*);
    void(*div)(void*,const void*);
    void(*mod)(void*,const void*);

    int(*cmp)(const void*,const void*);

    // Casts
    Table<intptr_t,Cast> casts;
    Cast cast(const TypeDescription* target) const; // Return Cast for target type if available or nullptr otherwise
  };

  template <class T>
  class Type {
  private:
    static TypeDescription td;
    static TypeDescription makeDesc() {
      TypeDescription wtr = {
        "",sizeof(T),
        cpy,cpyto,assign,dtr,del,out,Type<T>::hash,0
      };
#if defined _MSC_VER
      // "struct L::TypeDescription __cdecl L::Type<int>::makeDesc(void)"
      // "struct L::TypeDescription __cdecl L::Type<class L::String>::makeDesc(void)"
      char funcsig[] = __FUNCSIG__;
      int start = (!memcmp(funcsig+42,"class ",6)) ? 48 : 42;
      int end = strlen(funcsig)-17;
      if(funcsig[end-1]==' ') end--;
      funcsig[end] = '\0';
      strcpy(wtr.name,funcsig+start);
#else
      // "static L::TypeDescription L::Type<T>::makeDesc() [with T = XXX]"
      strcpy(wtr.name,__PRETTY_FUNCTION__+59);
      wtr.name[strlen(wtr.name)-1] = '\0';
#endif
      return wtr;
    }
    static void* cpy(void* p) { return new T(*(T*)p); }
    static void cpyto(void* dst,const void* src) { new((T*)dst) T(*(const T*)src); }
    static void assign(void* dst,const void* src) { *(T*)dst = *(const T*)src; }
    static void dtr(void* p) { ((T*)p)->~T(); }
    static void del(void* p) { delete(T*)p; }
    static void out(Stream& s,const void* p) { s << (*(const T*)p); }
    static uint32_t hash(const void* p) { return L::hash(*(const T*)p); }

  public:
    static inline const TypeDescription* description() { return &td; }
    static inline const char* name() { return td.name; }

    // Casts
    static inline void addcast(const TypeDescription* td,Cast cast){ Type::td.casts[(intptr_t)td] = cast; }
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

  // Instantiate structures
  template <class T> TypeDescription Type<T>::td(Type<T>::makeDesc());

  void TypeInit();
}