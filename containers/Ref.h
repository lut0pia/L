#ifndef DEF_L_Ref
#define DEF_L_Ref

#include <cstdlib>
#include <iostream>

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
      Ref() : p(NULL), c(NULL) {}
      Ref(const Ref& other) {
        copy(other);
      }
      template <class R>
      Ref(const Ref<R>& other) {
        copy(other);
      }
      template <class R>
      Ref(R* p): p(p), c(new int(1)) {}
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
      ~Ref() {
        free();
      }
      bool operator==(const Ref& other) {
        return (p == other.p);
      }
      const T& operator*() const {
        return *p;
      }
      T& operator*() {
        return *p;
      }
      operator T*() const {
        return p;
      }
      T* operator->() const {
        return p;
      }
      inline bool null() const {
        return (p==NULL);
      }
      inline void clear() {
        *this = Ref();
      }
      template <class R>
      friend class Ref;
  };

}

#endif
