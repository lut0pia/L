#pragma once

#include "../math/Interval.h"
#include "Pool.h"

namespace L {
  template <int d,class K,class V>
  class IntervalTree {
    private:
      typedef Interval<d,K> Key;
      class Node {
        private:
          Node* _parent;
          Key _key;
          union {
            V _value;
            struct {
              Node *_left,*_right;
            };
          };
          uint32_t _leaf;
        public:
          Node(Node* parent,const Key& key,const V& value) : _parent(parent),_key(key),_value(value) {
            _leaf = 1;
          }
          ~Node() {
            if(!_leaf) {
              delete _left;
              delete _right;
            }
          }
          inline const Key& key() const { return _key; }
          inline const V& value() const { return _value; }

          void insert(const Key& key,const V& value) {
          }

          inline void* operator new(size_t size) {return Pool<Node>::global.allocate();}
          inline void operator delete(void* p) { Pool<Node>::global.deallocate(p); }
      };
      Node* _root;

    public:
      IntervalTree() : _root(nullptr) {}
      ~IntervalTree() { delete _root; }
      void insert(const Key& key,const V& value) {
        if(_root) _root->insert(key,value);
        else _root = new Node(nullptr,key,value);
      }
  };

  // Regular trees
  template <class K,class V> class AABBTree : public IntervalTree<3,K,V> {};
}
