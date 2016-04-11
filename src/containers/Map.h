#ifndef DEF_L_Map
#define DEF_L_Map

#include "KeyValue.h"
#include "Set.h"
#include "../macros.h"

namespace L {
  template <class K,class V>
  class Map : public Set<KeyValue<K,V> > {
      typedef KeyValue<K,V> KV;
    public:
      template <class T>
      KV* find(const T& k) {
        int i(Set<KeyValue<K,V> >::index(k));
        if(i>=Array<KV>::size() || Array<KV>::operator[](i)>k)
          return 0;
        else return &Array<KV>::operator[](i);
      }
      template <class T>
      const KV* find(const T& k) const {
        int i(Set<KeyValue<K,V> >::index(k));
        if(i>=Array<KV>::size() || Array<KV>::operator[](i)>k)
          return 0;
        else return &Array<KV>::operator[](i);
      }
      template <class T>
      V& operator[](const T& k) {
        KV* e(find(k));
        if(e) return e->value();
        Set<KeyValue<K,V> >::insert(KV(k,V()));
        return operator[](k);
      }
      const V& operator[](const K& k) const {
        const KV* e(find(k));
        if(e) return e->value();
        L_ERROR("Couldn't find key in Map");
      }
      inline const KeyValue<K,V>& at(int i) const {return Set<KV>::operator[](i);}
      inline bool has(const K& k) const {return find(k)!=0;}
  };
  template <class K,class V> inline Stream& operator<<(Stream& s, const Map<K,V>& v) { return s << (const Array<KeyValue<K,V> >&)v;}
}

#endif

