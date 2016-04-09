#ifndef DEF_L_KeyValue
#define DEF_L_KeyValue

#include "../streams/Stream.h"

namespace L {
  template <class K, class V>
  class KeyValue {
    private:
      K _key;
      V _value;
    public:
      KeyValue(const K& key = K(), const V& value = V()) : _key(key), _value(value) {}
      template <class T> inline bool operator<(const T& other) const {return key()<other;}
      template <class T> inline bool operator>(const T& other) const {return key()>other;}
      inline bool operator<(const KeyValue& other) const {return key()<other.key();}
      inline bool operator>(const KeyValue& other) const {return key()>other.key();}
      template <class T> inline bool operator==(const T& other) const {return key()==other;}
      inline bool operator==(const KeyValue& other) const {return key()==other.key();}
      inline const K& key() const {return _key;}
      inline const V& value() const {return _value;}
      inline V& value() {return _value;}
  };
  template <class K, class V>
  inline KeyValue<K,V> keyValue(const K& k, const V& v) {
    return KeyValue<K,V>(k,v);
  }
  template <class K, class V> Stream& operator<<(Stream& s, const KeyValue<K,V>& v) {return s << '(' << v.key() << ':' << v.value() << ')';}
}

#endif



