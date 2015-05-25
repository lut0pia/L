#ifndef DEF_L_Object
#define DEF_L_Object

namespace L {
  namespace Object {
    template <class T, typename... Args>
    inline void construct(T& v, Args&&... args) {
      new(&v) T(args...);
    }
    template <class T>
    inline void destruct(T& v) {
      v.~T();
    }
    template <class T, typename... Args>
    inline void reconstruct(T& v, Args&&... args) {
      destruct(v);
      construct(v,args...);
    }
    template <class T, bool pod = false>
    inline void copy(T* dst, const T* src, size_t count) {
      while(count--)
        new(dst++) T(*src++);
    }
    template <class T>
    void swap(T& a, T& b) {
      char tmp[sizeof(T)];
      memcpy(tmp,&a,sizeof(T));
      memcpy(&a,&b,sizeof(T));
      memcpy(&b,tmp,sizeof(T));
    }
  }
}

#endif

