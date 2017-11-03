#pragma once

#include "../macros.h"
#include "../types.h"

#define L_ALLOCABLE(...) public: \
inline void* operator new(size_t SIZE) { L_ASSERT(SIZE==sizeof(__VA_ARGS__)); return L::Memory::allocType<__VA_ARGS__>();} \
inline void operator delete(void* P) { L::Memory::freeType((__VA_ARGS__*)P); }

void* operator new(size_t size);

namespace L {
  class Memory {
  public:
    template<typename T, typename... Args> static T* new_type(Args&&... args) { return new(allocType<T>())T(args...); }
    template<typename T> static T* allocType(size_t count = 1) { return (T*)alloc(sizeof(T)*count); }
    static void* alloc(size_t);
    static void* allocZero(size_t); // Allocates and zero-fill block
    static void* realloc(void*, size_t oldsize, size_t newsize);
    template<typename T> static void delete_type(T* p) { p->~T(); freeType(p); }
    template<typename T> static void freeType(T* p, size_t count = 1) { free(p, sizeof(T)*count); }
    static void free(void*, size_t);

    static void* virtualAlloc(size_t);
    static void virtualFree(void*, size_t);
  };
}
