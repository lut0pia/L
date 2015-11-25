#ifndef DEF_L_Ref
#define DEF_L_Ref

#include <cstdlib>

namespace L {
  template<class T>
  class Ref {
    private:
      T* p;
      int* c; // Reference counter

      template <class R>
      void copy(const Ref<R>& other) {
        p = other.p;
        c = other.c;
        if(c)(*c)++;
      }
      void free() {
        if(p) {
          (*c)--;
          if(*c==0) {
            delete p;
            delete c;
          }
        }
      }
    public:
      inline Ref() : p(NULL), c(NULL) {}
      inline Ref(const Ref& other) {copy(other);}
      template <class R> inline Ref(const Ref<R>& other) {copy(other);}
      template <class R> inline Ref(R* p): p(p), c(new int(1)) {}
      inline ~Ref() {free();}

      Ref& operator=(const Ref& other) {
        if(p != other.p) {
          free();
          copy(other);
        }
        return *this;
      }
      template <class R>
      Ref& operator=(const Ref<R>& other) {
        if(p != other.p) {
          free();
          p = other.p;
          c = other.c;
          if(c) *c++;
        }
        return *this;
      }
      inline bool operator==(const Ref& other) {return (p == other.p);}
      inline const T& operator*() const {return *p;}
      inline T& operator*() {return *p;}
      inline operator T*() const {return p;}
      inline T* operator->() const {return p;}
      inline bool null() const {return (p==NULL);}
      inline void clear() {free(); p=NULL; c=NULL;}

      template <class R> friend class Ref;
  };

}

#endif
