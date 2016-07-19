#pragma once

#include "../hash.h"
#include "KeyValue.h"

namespace L {
  template <class K,class V>
  class Table {
    typedef KeyValue<uint32_t,V> KV;
  protected:
    KV* _slots;
    size_t _size,_count;
    void grow(){
      if(_slots){
        size_t oldsize(_size);
        KV* oldslots(_slots);
        _slots = (KV*)calloc(_size *= 2,sizeof(KV));
        for(uintptr_t i(0); i<oldsize; i++)
          if(oldslots[i].key())
            memcpy(find(oldslots[i].key()),oldslots+i,sizeof(KV));
        free(oldslots);
      } else _slots = (KV*)calloc(_size = 4,sizeof(KV));
    }
    class Iterator{
    private:
      KV* _kv;
    public:
      inline Iterator(KV* kv) : _kv(kv){}
      inline Iterator& operator++(){ while(!(++_kv)->key()); return *this; }
      inline bool operator!=(const Iterator& other) const{ return _kv != other._kv; }
      inline KV* operator->() const{ return _kv; }
      inline KV& operator*() const{ return *(operator->()); }
    };
  public:
    inline Table() : _slots(nullptr),_size(0),_count(0){}
    Table(const Table& other) : _slots((KV*)calloc(other._size,sizeof(KV))),_size(other._size),_count(other._count){
      for(uintptr_t i(0); i<_size; i++)
        if(other._slots[i].key())
          new(_slots+i)KV(other._slots[i]);
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
            if(_slots[i].key())
              destruct(_slots[i].value());
        free(_slots);
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
      KV* slot(find(h));
      if(!slot->key()){
        construct(*slot,hash(k));
        _count++;
      }
      return slot->value();
    }
    KV* find(uint32_t h){
      do{
        const uintptr_t i(h*((float)_size/UINT32_MAX));
        for(uintptr_t j(0); j<_size; j++){
          const uintptr_t k((i+j)%_size);
          if(!_slots[k].key() || _slots[k].key()==h)
            return _slots+k;
        }
        grow();
      } while(true);
    }
  };
  template <class K,class V> inline Stream& operator<<(Stream& s,const Table<K,V>& v) {
    s << '[';
    bool first(true);
    for(auto&& e : v){
      if(first) first = false;
      else s << ',';
      s << e;
    }
    s << ']';
    return s;
  }
}
