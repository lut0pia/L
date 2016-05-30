#pragma once

#include "../objects.h"

namespace L {
  template <class T>
  class Pool {
    static const uint32_t tableSize = 16;
    static const uint32_t intBits = sizeof(uint32_t)*8;
  private:
    class Block {
    public:
      static const uint32_t size = tableSize*intBits;
      byte _data[size*sizeof(T)];
      uint32_t _table[tableSize]; // Every bit of this array represents a object slot in _data
      Block* _next; // Pool works like a linked list, in case no more space is available
      uint32_t _head,_tail;
      inline Block() : _table{0},_next(nullptr),_head(0),_tail(0){}
      inline ~Block() { delete _next; }
      inline bool allocated(uint32_t i) const{ return i<_head || (_table[i/intBits]&(1u<<(i%intBits)))!=0; } // Check if element i is allocated
      inline bool full() const{ return _head==size; }
      inline T* pointer(uint32_t i) const{ return ((T*)_data) + i; } // Returns pointer to ith element
      inline T* allocate() {
        uint32_t i(_head);
        while(allocated(++_head)); // Move head to first unallocated slot
        if(i>_tail) _tail = i;
        _table[i/intBits] |= 1<<(i%intBits); // Mark slot allocated
        return pointer(i); // Return pointer to element
      }
      inline void deallocate(uint32_t i) {
        if(i<_head) _head = i;
        if(_tail==i) _tail--;
        _table[i/intBits] &= ~(1u<<(i%intBits)); // Mark slot as free
      }
      inline void deallocate(void* p){ deallocate(((uintptr_t)p-(uintptr_t)_data)/sizeof(T)); }
      inline bool hasAddress(void *p) const{ return _data<=p && p<_data+sizeof(_data); }
    };
    class Iterator{
    private:
      Block* _block;
      uint32_t _i;

    public:
      Iterator() : _block(0),_i(0){}
      Iterator(Block* block) : _block(block),_i(0){}

      Iterator& operator++(){
        if(_block){
          do{
            if(++_i>_block->_tail){ // End of the block
              _block = _block->_next;
              _i = 0;
            }
          } while(_block && !_block->allocated(_i)); // Stop when no more block or allocated slot is found
        }
        return *this;
      }
      inline bool operator!=(const Iterator& other) const{
        if(!_block && !other._block) return false;
        return _block != other._block || _i != other._i;
      }
      inline T* operator->() const{ return (T*)_block->pointer(_i); }
      inline T& operator*() const{ return *(operator->()); }
    };
    Block* _root;

  public:
    inline Pool() : _root(nullptr) {}
    ~Pool() {
      for(auto&& e : *this)
        L::destruct(e);
      delete _root;
    }

    template <typename... Args>
    T* construct(Args&&... args) {
      T* wtr(allocate());
      L::construct(*wtr,args...);
      return wtr;
    }
    void destruct(T* o) {
      L::destruct(*o);
      deallocate(o);
    }
    T* allocate() {
      if(!_root) _root = new Block();
      for(Block* block(_root);; block = block->_next){
        if(!block->full()) return block->allocate();
        if(!block->_next) block->_next = new Block(); // Make sure there's a next block
      }
    }
    void deallocate(void* p) {
      for(Block* block(_root); block; block = block->_next) // Stop when the block doesn't exist
        if(block->hasAddress(p)) return block->deallocate(p);
    }
    inline Iterator begin() const{ return Iterator(_root); }
    inline Iterator end() const{ return Iterator(nullptr); }
    static Pool global;
  };
  template <class T> Pool<T> Pool<T>::global;
}
