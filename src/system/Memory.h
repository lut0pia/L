#pragma once

#include <cstdint> 
#include "../dev/debug.h"

#define L_ALLOCABLE(...) public: \
inline void* operator new(size_t SIZE) { L_ASSERT(SIZE==sizeof(__VA_ARGS__)); return L::Memory::alloc_type<__VA_ARGS__>();} \
inline void operator delete(void* P) { L::Memory::free_type((__VA_ARGS__*)P); }

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
