#ifndef DEF_L_Pool
#define DEF_L_Pool

#include "../tmp.h"

namespace L {
  template <class T>
  class Pool {
      static const int tableSize = 16;
      static const int intBits = sizeof(int)*8;
      static const int typeSize = sizeof(T);
      static const int alignedTypeSize = align<typeSize,__alignof(T)>::value;
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
          void* allocate() {
            byte* wtr(_data);
            if(!_full)
              for(int i(0); i<tableSize; i++)
                if(_table[i]!=~0) { // This part of the block is not full
                  for(int j(0); j<intBits; j++)
                    if(_table[i]&(1<<j))
                      wtr += alignedTypeSize;
                    else {
                      _table[i] |= 1<<j;
                      return wtr;
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
      };
      static Block* _root;

    public:
      static void* allocate() {
        if(!_root)
          _root = new Block();
        return _root->allocate();
      }
      static void deallocate(void* p) {
        if(_root)
          _root->deallocate(p);
      }
  };
  template <class T>
  typename Pool<T>::Block* Pool<T>::_root(NULL);
}

#endif



