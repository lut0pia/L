#pragma once

#include "../tmp.h"
#include "../objects.h"

namespace L {
  template <class T>
  class Pool {
    static const int tableSize = 16;
    static const int intBits = sizeof(int)*8;
  private:
    class Block {
    public:
      static const int size = tableSize*intBits;
      byte _data[size*sizeof(T)];
      uint32_t _table[tableSize]; // Every bit of this array represents a object slot in _data
      Block* _next; // Pool works like a linked list, in case no more space is available
      bool _full;
      inline Block() : _table{0},_next(nullptr),_full(false){}
      inline ~Block() { delete _next; }
      inline bool allocated(int i) const{ return (_table[i/intBits]&(1<<(i%intBits)))!=0; } // Check if element i is allocated
      inline T* pointer(int i) const{ return ((T*)_data) + i; } // Returns pointer to ith element
      inline T* allocate(int i) {
        _table[i/intBits] |= 1<<(i%intBits); // Mark slot allocated
        return pointer(i); // Return pointer to element
      }
      inline void deallocate(int i) {
        _table[i/intBits] &= ~(1<<(i%intBits)); // Mark slot as free
        _full = false; // The block is not full
      }
      inline bool hasAddress(void *p) const{ return _data<=p && p<_data+sizeof(_data); }
    };
    class Iterator{
    private:
      Block* _block;
      int _i;

    public:
      Iterator() : _block(0),_i(0){}
      Iterator(Block* block) : _block(block),_i(0){}

      Iterator& operator++(){
        if(_block){
          do{
            if(++_i==Block::size){ // End of the block
              _block = _block->_next;
              _i = 0;
              if(!_block) return *this; // No more blocks
            }
          } while(!_block->allocated(_i)); // Stop when allocated slot is found
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
      Block* block(_root); // Start with the root block
      while(true){ // Repeat until allocated
        if(!block->_full){
          for(int i(0); i<Block::size; i++)
            if(!block->allocated(i)) // This slot is not allocated
              return block->allocate(i); // Allocate that slot
          block->_full = true; // If here then the block is full
        }
        if(!block->_next) block->_next = new Block(); // Make sure there's a next block
        block = block->_next; // Continue with next block
      }
    }
    void deallocate(void* p) {
      Block* block(_root);
      while(block){ // Stop when the block doesn't exist
        if(block->hasAddress(p)) { // Pointer is in this block
          int i(((intptr_t)p-(intptr_t)block->_data)/sizeof(T)); // Compute element's index
          block->deallocate(i);
        } else block = block->_next;
      }
    }
    inline Iterator begin() const{ return Iterator(_root); }
    inline Iterator end() const{ return Iterator(nullptr); }
    static Pool global;
  };
  template <class T> Pool<T> Pool<T>::global;
}
