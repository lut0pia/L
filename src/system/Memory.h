#pragma once

#include <cstdint> 
#include <new>
#include "../dev/debug.h"

namespace L {
  class Memory {
  public:
    template<typename T, typename... Args> static T* new_type(Args&&... args) { return new(alloc_type<T>())T(args...); }
    template<typename T> static T* alloc_type(size_t count = 1) { return (T*)alloc(sizeof(T)*count); }
    template<typename T> static T* alloc_type_zero(size_t count = 1) { return (T*)alloc_zero(sizeof(T)*count); }
    static void* alloc(size_t);
    static void* alloc_zero(size_t); // Allocates and zero-fill block
    static void* realloc(void*, size_t oldsize, size_t newsize);
    template<typename T> static void delete_type(T* p) { p->~T(); free_type(p); }
    template<typename T> static void free_type(T* p, size_t count = 1) { free(p, sizeof(T)*count); }
    static void free(void*, size_t);

    static void* virtual_alloc(size_t);
    static void virtual_free(void*, size_t);
  };
}
