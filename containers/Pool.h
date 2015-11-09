#ifndef DEF_L_Pool
#define DEF_L_Pool

#include <functional>
#include "../tmp.h"
#include "../objects.h"

namespace L {
  template <class T>
  class Pool {
      static const int tableSize = 16;
      static const int intBits = sizeof(int)*8;
      static const int alignedTypeSize = alignment<T>::padded;
      static const int blockSize = tableSize*intBits*alignedTypeSize;

    private:
      class Block {
        private:
          int _table[tableSize]; // Every bit of this array represents a object slot in _data
          byte _data[blockSize];
          Block* _next; // Pool works like a linked list, in case no more space is available
          bool _full;
        public:
          Block() : _next(NULL), _full(false) {
            for(int i(0); i<tableSize; i++)
              _table[i] = 0;
          }
          ~Block() {
            delete _next;
          }
          T* allocate() {
            byte* wtr(_data);
            if(!_full)
              for(int i(0); i<tableSize; i++)
                if(_table[i]!=~0) { // This part of the block is not full
                  for(int j(0); j<intBits; j++)
                    if(_table[i]&(1<<j))
                      wtr += alignedTypeSize;
                    else {
                      _table[i] |= 1<<j;
                      return (T*)wtr;
                    }
                } else wtr += alignedTypeSize*intBits; // This part of the block is full
            _full = true;
            if(!_next)
              _next = new Block();
            return _next->allocate();
          }
          void deallocate(void* p) {
            if(_data<=p && p<=_data+sizeof(_data)) {
              int i(((int)p-(int)_data)/alignedTypeSize);
              int j(i%intBits); // Bit position
              i /= intBits; // Position in table
              _table[i] &= ~(1<<j);
              _full = false;
            } else if(_next)
              _next->deallocate(p);
          }
          bool allocated(void* p) {
            if(_data<=p && p<=_data+sizeof(_data)) {
              int i(((int)p-(int)_data)/alignedTypeSize);
              int j(i%intBits); // Bit position
              i /= intBits; // Position in table
              return (_table[i]&(1<<j));
            } else if(_next)
              return _next->allocated(p);
            else
              return false;
          }
          void foreach(const std::function<void(T&)>& f) {
            byte* ptr(_data);
            for(int i(0); i<tableSize; i++)
              if(_table[i]!=0) { // This part of the block is not empty
                for(int j(0); j<intBits; j++) {
                  if(_table[i]&(1<<j))
                    f(*(T*)ptr);
                  ptr += alignedTypeSize;
                }
              } else ptr += alignedTypeSize*intBits; // This part of the block is empty
            if(_next)
              _next->foreach(f);
          }
      };
      Block* _root;

    public:
      inline Pool() : _root(NULL) {}
      inline ~Pool() {delete _root;}

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
        if(!_root)
          _root = new Block();
        return _root->allocate();
      }
      inline void deallocate(void* p) {
        if(_root) _root->deallocate(p);
      }
      inline bool allocated(void* p) {
        if(_root) return _root->allocated(p);
        else return false;
      }
      inline void foreach(const std::function<void(const T&)>& f) const {
        if(_root) _root->foreach(f);
      }
      inline void foreach(const std::function<void(T&)>& f) {
        if(_root) _root->foreach(f);
      }
  };
}

#endif



