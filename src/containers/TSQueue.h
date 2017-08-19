#pragma once

#include <atomic>
#include "../macros.h"
#include "../types.h"

namespace L {
  template <int n, class T>
  class TSQueue {
    static_assert(n && !(n & (n - 1)), "TSQueue size should be a power of two");
  private:
    std::atomic<uint32_t> _writeHead, _writeTail, _readHead, _readTail;
    byte _array[n*sizeof(T)];

    inline T* at(uint32_t i) { return (T*)_array+(i%n); }

  public:
    inline TSQueue() : _writeHead(0), _writeTail(0), _readHead(0), _readTail(0), _array{} {}
    void push(const T& e) {
      // Assume queue cannot be full
      const uint32_t writeIndex(_writeHead.fetch_add(1));
      new(at(writeIndex))T(e);
      uint32_t writeIndexExp;
      do writeIndexExp = writeIndex;
      while(!_writeTail.compare_exchange_weak(writeIndexExp, writeIndex+1, std::memory_order_release, std::memory_order_relaxed));
    }
    bool pop(T& e) {
      const uint32_t readIndex(_readHead);
      uint32_t readIndexExp(readIndex);
      if(readIndex<_writeTail && _readHead.compare_exchange_weak(readIndexExp, readIndex+1, std::memory_order_release, std::memory_order_relaxed)) {
        e = *at(readIndex);
        at(readIndex)->~T();
        do readIndexExp = readIndex;
        while(!_readTail.compare_exchange_weak(readIndexExp, readIndex+1, std::memory_order_release, std::memory_order_relaxed));
        return true;
      } else return false;
    }
  };
}
