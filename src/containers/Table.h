#pragma once

#include "../hash.h"
#include "KeyValue.h"
#include "List.h"

namespace L {
  template <class K,class V,uint32_t S = 389>
  class Table {
    typedef KeyValue<uint32_t,V> KV;
    List<KV> _slots[S];
  public:
    V& operator[](const K& k){
      uint32_t h(hash(k)),i(h%S);
      for(auto&& slot : _slots[i])
        if(slot.key()==h)
          return slot.value();
      _slots[i].push(h);
      return _slots[i].back().value();
    }
  };
}
