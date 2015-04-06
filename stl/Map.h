#ifndef DEF_L_Map
#define DEF_L_Map

#include <map>
#include "../Exception.h"

namespace L {
  template <class K, class V>
  class Map : public std::map<K,V> {
    public:
      Map() : std::map<K,V>() {}
      Map(const std::map<K,V>& other) : std::map<K,V>(other) {}

      using std::map<K,V>::operator[];
      const V& operator[](const K& e) const {
        typename std::map<K,V>::const_iterator it(std::map<K,V>::find(e));
        if(it != std::map<K,V>::end())
          return it->second;
        else throw Exception("Key not found in Map");
      }
      inline bool has(const K& e) const {
        return (std::map<K,V>::find(e) != std::map<K,V>::end());
      }

  };
}

#endif

