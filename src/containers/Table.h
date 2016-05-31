#pragma once

#include "../hash.h"
#include "KeyValue.h"
#include "List.h"

namespace L {
  template <class K,class V,uint32_t S = 389>
  class Table {
    typedef KeyValue<K,V> KV;
    List<KV> _slots[S];
  public:
    V& operator[](const K& k){
      auto index(hash(k)%S);
      for(auto&& slot : _slots[index])
        if(slot.key()==k)
          return slot.value();
      _slots[index].push(k);
      return _slots[index].back().value();
    }
  };
}
