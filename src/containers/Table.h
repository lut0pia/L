#pragma once

#include "../hash.h"
#include "../system/Memory.h"
#include "../streams/Stream.h"

namespace L {
  template <class K,class V>
  class Table {
  public:
    class Slot{
    private:
      struct Layout{
        uint32_t _hash;
        K _key;
        V _value;
      };
      byte _data[sizeof(Layout)];
      inline Layout& layout() { return *(Layout*)&_data; }
      inline const Layout& layout() const { return *(Layout*)&_data; }
      inline uint32_t& hash() { return layout()._hash; }
    public:
      inline Slot(uint32_t h,const K& key) {
        hash() = h;
        new(&layout()._key)K(key);
        new(&layout()._value)V();
      }
      inline bool empty() const { return !hash(); }
      inline uint32_t hash() const { return layout()._hash; }
      inline const K& key() const { return layout()._key; }
      inline const V& value() const { return layout()._value; }
      inline V& value() { return layout()._value; }
      friend Table;
    };
  protected:
    Slot* _slots;
    size_t _size,_count;
    void grow(){
      if(_slots){
        size_t oldsize(_size);
        Slot* oldslots(_slots);
        _slots = (Slot*)Memory::allocZero((_size *= 2)*sizeof(Slot));
        for(uintptr_t i(0); i<oldsize; i++)
          if(!oldslots[i].empty())
            memcpy(find(oldslots[i].hash()),oldslots+i,sizeof(Slot));
        Memory::free(oldslots,oldsize*sizeof(Slot));
      } else _slots = (Slot*)Memory::allocZero((_size = 4)*sizeof(Slot));
    }
    class Iterator{
    private:
      Slot* _slot;
    public:
      inline Iterator(Slot* slot) : _slot(slot){}
      inline Iterator& operator++(){ while(!(++_slot)->hash()); return *this; }
      inline bool operator!=(const Iterator& other) const { return _slot != other._slot; }
      inline Slot* operator->() const{ return _slot; }
      inline Slot& operator*() const{ return *(operator->()); }
    };
  public:
    inline Table() : _slots(nullptr),_size(0),_count(0){}
    Table(const Table& other) : _slots((Slot*)(other._slots?Memory::allocZero(other._size*sizeof(Slot)):nullptr)),_size(other._size),_count(other._count){
      for(uintptr_t i(0); i<_size; i++)
        if(!other._slots[i].empty())
          new(_slots+i)Slot(other._slots[i]);
    }
    inline Table& operator=(const Table& other){
      this->~Table();
      new(this)Table(other);
      return *this;
    }
    ~Table(){
      if(_slots){
        if(_count)
          for(uintptr_t i(0); i<_size; i++)
            if(!_slots[i].empty())
              _slots[i].~Slot();
        Memory::free(_slots,_size*sizeof(Slot));
      }
    }
    inline size_t size() const{ return _size; }
    inline size_t count() const{ return _count; }
    inline Iterator begin() const{ return (_count) ? ++Iterator(_slots-1) : Iterator(nullptr); }
    inline Iterator end() const{ return (_count) ? _slots+_size : Iterator(nullptr); }
    V& operator[](const K& k){
      if(_count*10>=_size*7)
        grow();
      uint32_t h(hash(k));
      Slot* slot(find(h));
      if(slot->empty()){
        new(slot)Slot(h,k);
        _count++;
      }
      return slot->value();
    }
    Slot* find(uint32_t h){
      do{
        const uintptr_t i(h*((float)_size/UINT32_MAX));
        for(uintptr_t j(0); j<_size; j++){
          const uintptr_t k((i+j)%_size);
          if(_slots[k].empty() || _slots[k].hash()==h)
            return _slots+k;
        }
        grow();
      } while(true);
    }
    Slot* find(const K& key) const {
      const uint32_t h(hash(key));
      const uintptr_t i(h*((float)_size/UINT32_MAX));
      for(uintptr_t j(0); j<_size; j++){
        const uintptr_t k((i+j)%_size);
        if(_slots[k].hash()==h)
          return _slots+k;
        else if(_slots[k].empty())
          return nullptr;
      }
      return nullptr;
    }
  };
  template <class K,class V> inline Stream& operator<<(Stream& s,const Table<K,V>& v) {
    s << '{';
    bool first(true);
    for(auto&& e : v){
      if(first) first = false;
      else s << ',';
      s << e.key() << ':' << e.value();
    }
    s << '}';
    return s;
  }
}
