#pragma once

#include "../types.h"

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
      uint32_t _start,_head,_tail; // First allocated, first unallocated, last allocated
      inline Block() : _table{0},_next(nullptr),_start(size-1),_head(0),_tail(0){}
      inline ~Block() { delete _next; }
      inline bool allocated(uint32_t i) const{ return i<_head || (_table[i/intBits]&(1u<<(i%intBits)))!=0; } // Check if element i is allocated
      inline bool full() const{ return _head==size; }
      inline T* pointer(uint32_t i) const{ return ((T*)_data) + i; } // Returns pointer to ith element
      inline T* allocate() {
        uint32_t i(_head);
        _table[i/intBits] |= 1<<(i%intBits); // Mark slot allocated
        if(i<_start) _start = i;
        if(i>_tail) _tail = i;
        while(_head<size && allocated(_head)) _head++;
        return pointer(i); // Return pointer to element
      }
      inline void deallocate(uint32_t i) {
        _table[i/intBits] &= ~(1u<<(i%intBits)); // Mark slot as free
        if(i<_head) _head = i;
        while(_start<size && !allocated(_start)) _start++;
        while(_tail>0 && !allocated(_tail)) _tail--;
      }
      inline void deallocate(void* p){ deallocate(((uintptr_t)p-(uintptr_t)_data)/sizeof(T)); }
      inline bool hasAddress(void *p) const{ return _data<=p && p<_data+sizeof(_data); }
    };
    class Iterator{
    private:
      Block* _block;
      uint32_t _i;

    public:
      Iterator(Block* block = nullptr) : _block(block){
        if(_block){
          if(_block->_start<=_block->_tail)
            _i = _block->_start;
          else new(this)Iterator(_block->_next);
        }
      }

      Iterator& operator++(){
        if(_block){
          do{
            if(++_i>_block->_tail) // End of the block
              new(this)Iterator(_block->_next);
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
        e.~T();
      delete _root;
    }
    void clear(){
      for(auto&& e : *this)
        destruct(&e);
    }

    template <typename... Args>
    inline T* construct(Args&&... args) {
      return new(allocate())T(args...);
    }
    inline void destruct(T* o) {
      o->~T();
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
    inline Iterator end() const{ return Iterator(); }
    static Pool global;
  };
  template <class T> Pool<T> Pool<T>::global;
}
