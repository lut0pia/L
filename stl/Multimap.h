#ifndef DEF_L_STL_Multimap
#define DEF_L_STL_Multimap

#include <map>
#include "../Exception.h"

namespace L {
  template <class K, class V>
  class Multimap : public std::multimap<K,V> {
    public:
      Multimap() : std::multimap<K,V>() {}
      Multimap(const std::multimap<K,V>& other) : std::multimap<K,V>(other) {}

      bool has(const K& k) const {
        return std::multimap<K,V>::find(k) != std::multimap<K,V>::end();
      }
      Set<V> operator[](const K& k) { // Returns all elements having k as a key
        Set<V> wtr;
        typename std::multimap<K,V>::iterator it(std::multimap<K,V>::find(k));
        while(it != std::multimap<K,V>::end() && it->first==k) {
          wtr.insert(it->second);
          it++;
        }
        return wtr;
      }

  };
}

#endif


