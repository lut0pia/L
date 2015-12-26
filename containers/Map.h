#ifndef DEF_L_Map
#define DEF_L_Map

#include "KeyValue.h"
#include "Set.h"
#include "../Exception.h"

namespace L {
  template <class K,class V>
  class Map : public Set<KeyValue<K,V> > {
      typedef KeyValue<K,V> T;
    public:
      KeyValue<K,V>* find(const K& k) {
        KeyValue<K,V> key(k);
        int i(index(key));
        if(i>=Array<T>::size() || key<Array<T>::operator[](i))
          return 0;
        else return &Array<T>::operator[](i);
      }
      const KeyValue<K,V>* find(const K& k) const {
        KeyValue<K,V> key(k);
        int i(index(key));
        if(i>=Array<T>::size() || key<Array<T>::operator[](i))
          return 0;
        else return &Array<T>::operator[](i);
      }
      V& operator[](const K& k) {
        KeyValue<K,V>* e(find(k));
        if(e) return e->value();
        insert(KeyValue<K,V>(k,V()));
        return operator[](k);
      }
      const V& operator[](const K& k) const {
        const KeyValue<K,V>* e(find(k));
        if(e) return e->value();
        throw Exception("Couldn't find key in Map");
      }
      inline const KeyValue<K,V>& at(int i) const {return Set<KeyValue<K,V> >::operator[](i);}
      inline bool has(const K& k) const {return find(k)!=0;}
  };
  template <class K,class V> inline Stream& operator<<(Stream& s, const Map<K,V>& v) { return s << (const Array<KeyValue<K,V> >&)v;}
}

#endif

