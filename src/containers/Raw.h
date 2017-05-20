#pragma once

namespace L {
  template<size_t size>
  class Raw {
  protected:
    char _data[size];
  public:
    template<class T> inline T& as() {
      static_assert(sizeof(T)<=size, "Not enough raw bytes");
      return *(T*)_data;
    }
    template<class T> inline const T& as() const {
      static_assert(sizeof(T)<=size, "Not enough raw bytes");
      return *(const T*)_data;
    }
    template<class T> inline T& at(uintptr_t i) { return *((T*)_data+i); }
    template<class T> inline const T& at(uintptr_t i) const { return *((const T*)_data+i); }
  };
}
