#pragma once

#include "../String.h"
#include "../streams/Stream.h"

namespace L {
  // Structure to keep basic functions for a type
  typedef struct {
    char name[256];
    int size;

    void* (*cpy)(void*);
    void (*cpyto)(void*,const void*);
    void (*assign)(void*,const void*);
    void (*dtr)(void*);
    void (*del)(void*);

    void (*out)(Stream&,const void*);
    int (*cmp)(const void*,const void*);

    bool (*hascmp)();
  } TypeDescription;
  template <class T>
  class Type {
    private:
      static const TypeDescription td;
      static TypeDescription makeDesc() {
        TypeDescription wtr = {
          "",sizeof(T),
          cpy,cpyto,assign,dtr,del,
          out,cmp,
          hascmp
        };
#if defined _MSC_VER
        // "L::TypeDescription __cdecl L::Type<float>::makeDesc(void)
        // "L::TypeDescription __cdecl L::Type<class L::String>::makeDesc(void)
        char funcsig[] = __FUNCSIG__;
        int start = (!memcmp(funcsig+35,"class ",6))?41:35;
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

    public:
      static inline const TypeDescription* description() {return &td;}

      static void* cpy(void* p) {return new T(*(T*)p);}
      static void cpyto(void* dst,const void* src) { new((T*)dst) T(*(const T*)src); }
      static void assign(void* dst,const void* src) { *(T*)dst = *(const T*)src; }
      static void dtr(void* p) {((T*)p)->~T();}
      static void del(void* p) {delete(T*)p;}

      inline static const char* name() {return td.name;}
      static void out(Stream& s, const void* p) {s << (*(const T*)p);}
      static int cmp(const void* a, const void* b) {return 0;}

      static bool hasout() {return false;}
      static bool hascmp() {return false;}
  };

  // Instantiate structures
  template <class T> const TypeDescription Type<T>::td = Type<T>::makeDesc();

  // Specify void doesn't construct nor destruct
  template <> inline void* Type<void>::cpy(void* p) {return nullptr;}
  template <> inline void Type<void>::del(void* p) {}

#define L_TYPE_CMPABLE(T) template <> inline int Type< T >::cmp(const void* a, const void* b){\
    if((*(T*)a)<(*(T*)b))       return -1;\
    else if((*(T*)a)==(*(T*)b)) return 0;\
    else                        return 1;} \
  template <> inline bool Type< T >::hascmp(){return true;}

#define L_TYPE_PRIMITIVE(T) L_TYPE_CMPABLE(T)

  L_TYPE_PRIMITIVE(bool)
  L_TYPE_PRIMITIVE(unsigned char)
  L_TYPE_PRIMITIVE(char)
  L_TYPE_PRIMITIVE(unsigned short)
  L_TYPE_PRIMITIVE(short)
  L_TYPE_PRIMITIVE(unsigned int)
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



