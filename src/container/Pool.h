#pragma once

#include "../system/Memory.h"

namespace L {
  template <class T>
  class Pool {
  protected:
    static const size_t ideal_byte_size = 4*1024u;
    struct Slot {
      Slot* _next;
      uint8_t _padding[sizeof(T)-sizeof(Slot*)];
    };
    static_assert(sizeof(Slot)==sizeof(T), "Malformed Pool Slot");

    class Block {
      static const size_t slot_count = (ideal_byte_size-sizeof(Block*)) / sizeof(Slot);
    protected:
      Slot _slots[slot_count];
      Block* _next;
    public:
      Block(Block* next) : _next(next) {
        for(uintptr_t i(0); i<slot_count-1; i++)
          _slots[i]._next = _slots+i+1;
        _slots[slot_count-1]._next = nullptr;
      }
      inline Slot* first_slot() { return _slots; }
      inline ~Block() { if(_next) Memory::delete_type(_next); }
    };
    static_assert(sizeof(Block)<=ideal_byte_size, "Pool block size too big");

    Block* _block;
    Slot* _freelist;
    size_t _size;

  public:
    constexpr Pool() : _block(nullptr), _freelist(nullptr), _size(0) {}
    inline ~Pool() {
      if(_block) Memory::delete_type(_block);
    }

    template <typename... Args>
    inline T* construct(Args&&... args) {
      return ::new(allocate())T(args...);
    }
    inline void destruct(T* o) {
      o->~T();
      deallocate(o);
    }
    T* allocate() {
      _size++;
      if(!_freelist) {
        _block = Memory::new_type<Block>(_block);
        _freelist = _block->first_slot();
        L_ASSERT(_freelist);
      }
      Slot* p(_freelist);
      _freelist = _freelist->_next;
      return (T*)p;
    }
    void deallocate(void* p) {
      Slot* slot((Slot*)p);
      slot->_next = _freelist;
      _freelist = slot;
      _size--;
    }
  };
}
