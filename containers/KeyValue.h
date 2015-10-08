#ifndef DEF_L_KeyValue
#define DEF_L_KeyValue

namespace L {
  template <class K, class V>
  class KeyValue {
    private:
      K _key;
      V _value;
    public:
      KeyValue(const K& key = K(), const V& value = V()) : _key(key), _value(value) {}
      inline bool operator<(const KeyValue& other) const {return key()<other.key();}
      inline bool operator==(const KeyValue& other) const {return key()==other.key();}
      inline const K& key() const {return _key;}
      inline const V& value() const {return _value;}
      inline V& value() {return _value;}
  };
  template <class K, class V>
  inline KeyValue<K,V> keyValue(const K& k, const V& v) {
    return KeyValue<K,V>(k,v);
  }
}

#endif



