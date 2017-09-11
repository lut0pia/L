#pragma once

#include "KeyValue.h"
#include "Set.h"

namespace L {
  template <class K, class V>
  class Map : public Set<KeyValue<K, V>> {
    typedef KeyValue<K, V> KV;
  public:
    constexpr Map() {}
    inline Map(const std::initializer_list<KV>& il) {
      Array<KV>::growTo(il.size());
      for(const KV& e : il)
        insert(e);
    }
    KV* find(const K& k) {
      int i(Set<KV>::index(k));
      if(i>=(int)Array<KV>::size() || Array<KV>::operator[](i)>k)
        return 0;
      else return &Array<KV>::operator[](i);
    }
    const V* find(const K& k) const {
      int i(Set<KV>::index(k));
      if(i>=(int)Array<KV>::size() || Array<KV>::operator[](i)>k)
        return 0;
      else return &Array<KV>::operator[](i).value();
    }
    V& operator[](const K& k) {
      KV* e(find(k));
      if(e) return e->value();
      Set<KV>::insert(KV(k, V()));
      return operator[](k);
    }
    inline const KV& at(int i) const { return Set<KV>::operator[](i); }
    inline bool has(const K& k) const { return find(k)!=0; }

    friend inline Stream& operator<<(Stream& s, const Map& v) { return s << (const Array<KV>&)v; }
    friend inline Stream& operator<(Stream& s, const Map& v) { return s < (const Array<KV>&)v; }
    friend inline Stream& operator>(Stream& s, Map& v) { return s > (Array<KV>&)v; }
  };
}
