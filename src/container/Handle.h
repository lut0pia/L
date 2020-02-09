#pragma once

#include <cstdint>
#include <type_traits>

namespace L {
  template<class T> class Handled;

  class GenericHandle {
  protected:
    uint64_t _ver_index = UINT64_MAX;

    GenericHandle(void*);
    void release();

  public:
    constexpr GenericHandle() {}
    void* pointer() const;

    inline bool operator==(const GenericHandle& other) const { return _ver_index == other._ver_index; }
    inline bool operator!=(const GenericHandle& other) const { return _ver_index != other._ver_index; }
    inline bool operator>(const GenericHandle& other) const { return _ver_index > other._ver_index; }
    inline bool operator<(const GenericHandle& other) const { return _ver_index < other._ver_index; }

    inline bool is_valid() const { return pointer() != nullptr; }

    friend class GenericHandled;
  };

  class GenericHandled {
  private:
    GenericHandle _handle;

  protected:
    GenericHandled(void* ptr) {
      _handle = GenericHandle(ptr);
    }
    ~GenericHandled() {
      _handle.release();
    }

  public:
    inline const GenericHandle& generic_handle() const { return _handle; }
  };

  template<class T>
  class Handle : public GenericHandle {
  protected:
    inline void release() { GenericHandle::release(); }
    inline explicit Handle(T* ptr) : GenericHandle(ptr) {}

  public:
    constexpr Handle() : GenericHandle() {}
    inline explicit Handle(GenericHandle gen_handle) : GenericHandle(gen_handle) {}
    template <class R> inline Handle(const Handle<R>& other) {
      static_assert(std::is_base_of<T, R>::value, "Cannot convert to a non-base class");
      _ver_index = other._ver_index;
    }

    inline const T* operator->() const { return (const T*)pointer(); }
    inline T* operator->() { return (T*)pointer(); }
    inline const T& operator*() const { return *(const T*)pointer(); }
    inline T& operator*() { return *(T*)pointer(); }
    inline operator T*() { return (T*)pointer(); }
    inline operator const T*() const { return (const T*)pointer(); }
    inline operator bool() const { return is_valid(); }

    friend Handled<T>;
    template <class R> friend class Handle;
  };

  template<class T>
  class Handled {
  protected:
    Handle<T> _handle;
    Handled(T* ptr) {
      _handle = Handle<T>(ptr);
    }
    ~Handled() {
      _handle.release();
    }

  public:
    inline const Handle<T>& handle() const { return _handle; }
  };
}
