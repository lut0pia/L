#pragma once

#include "../String.h"
#include "../streams/Stream.h"
#include "../types.h"

namespace L {
  // Structure to keep basic functions for a type
  typedef struct {
    // Mandatory
    char name[256];
    int size;

    void* (*cpy)(void*);
    void(*cpyto)(void*,const void*);
    void(*assign)(void*,const void*);
    void(*dtr)(void*);
    void(*del)(void*);
    void(*out)(Stream&,const void*);

    // Optional
    void(*add)(void*,const void*);
    void(*sub)(void*,const void*);
    void(*mul)(void*,const void*);
    void(*div)(void*,const void*);
    void(*mod)(void*,const void*);

    int(*cmp)(const void*,const void*);
  } TypeDescription;

  template <class T>
  class Type {
  private:
    static TypeDescription td;
    static TypeDescription makeDesc() {
      TypeDescription wtr = {
        "",sizeof(T),
        cpy,cpyto,assign,dtr,del,out,0
      };
#if defined _MSC_VER
      // "L::TypeDescription __cdecl L::Type<float>::makeDesc(void)
      // "L::TypeDescription __cdecl L::Type<class L::String>::makeDesc(void)
      char funcsig[] = __FUNCSIG__;
      int start = (!memcmp(funcsig+35,"class ",6)) ? 41 : 35;
      int end = strlen(funcsig)-17;
      if(funcsig[end-1]==' ') end--;
      funcsig[end] = '\0';
      strcpy(wtr.name,funcsig+start);
#else
      // "static L::TypeDescription L::Type<T>::makeDesc() [with T = XXX]"
      char prettyfunc[] = __PRETTY_FUNCTION__;
      prettyfunc[strlen(prettyfunc)-1] = '\0';
      strcpy(wtr.name,prettyfunc+55);
#endif
      return wtr;
    }
    static void* cpy(void* p) { return new T(*(T*)p); }
    static void cpyto(void* dst,const void* src) { new((T*)dst) T(*(const T*)src); }
    static void assign(void* dst,const void* src) { *(T*)dst = *(const T*)src; }
    static void dtr(void* p) { ((T*)p)->~T(); }
    static void del(void* p) { delete(T*)p; }
    static void out(Stream& s,const void* p) { s << (*(const T*)p); }

  public:
    static inline const TypeDescription* description() { return &td; }
    static inline const char* name() { return td.name; }

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
        else if((*(T*)a)==(*(T*)b)) return 0;
        else                        return 1;
      });
    }
  };

  // Instantiate structures
  template <class T> TypeDescription Type<T>::td = Type<T>::makeDesc();

  inline void TypeInit(){
    Type<uint8_t>::canall<>();
    Type<int8_t>::canall<>();
    Type<uint16_t>::canall<>();
    Type<int16_t>::canall<>();
    Type<uint32_t>::canall<>();
    Type<int32_t>::canall<>();
    Type<uint64_t>::canall<>();
    Type<int64_t>::canall<>();
    Type<float>::canmath<>();
    Type<float>::cancmp<>();
    Type<double>::canmath<>();
    Type<double>::cancmp<>();
    Type<long double>::canmath<>();
    Type<long double>::cancmp<>();
    Type<String>::canadd<>();
    Type<String>::cancmp<>();
  }
}