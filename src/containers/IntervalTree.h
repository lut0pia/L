#pragma once

#include "../math/Interval.h"
#include "Pool.h"

namespace L {
  template <int d,class K,class V>
  class IntervalTree {
  public:
    typedef Interval<d,K> Key;
    class Node {
    private:
      Node *_parent,*_left,*_right;
      Key _key;
      int _height; // Used for balancing
      V _value;
      bool _crossed; // Used to avoid doubles in collisions
    public:
      inline Node(const Key& key,const V& value) : _parent(nullptr),_left(nullptr),_key(key),_height(0),_value(value) {}
      inline Node(Node* parent,Node* oldNode,Node* newNode) : _parent(parent),_left(oldNode),_right(newNode){
        if(parent)
          parent->replace(oldNode,this);
        _left->_parent = _right->_parent = this;
      }
      inline ~Node() {
        if(branch()) {
          delete _left;
          delete _right;
        }
      }
      inline bool branch() const{ return _left!=nullptr; }
      inline bool leaf() const{ return _left==nullptr; }
      inline Node* childNot(Node* node){ return (_left!=node) ? _left : _right; }
      inline Node* sibling(){ return _parent->childNot(this); }
      inline void replace(Node* oldNode,Node* newNode){
        ((_left==oldNode) ? _left : _right) = newNode;
        newNode->_parent = this;
      }
      inline const Key& key() const { return _key; }
      inline const V& value() const { return _value; }

      inline void* operator new(size_t size) { return Pool<Node>::global.allocate(); }
      inline void operator delete(void* p) { Pool<Node>::global.deallocate(p); }
      friend IntervalTree;
    };
  private:
    Node* _root;
  public:
    inline IntervalTree() : _root(nullptr) {}
    inline ~IntervalTree() { delete _root; }
    Node* insert(const Key& key,const V& value) {
      Node* node(new Node(key,value));
      if(_root){
        Node** cur(&_root);
        while((*cur)->branch()){
          const Node *left((*cur)->_left),*right((*cur)->_right);
          const Key& leftKey(left->_key),rightKey(right->_key);
          const Key newLeft(leftKey+key),newRight(rightKey+key);
          const float extentDiffLeft(newLeft.extent()-leftKey.extent()),
            extentDiffRight(newRight.extent()-rightKey.extent());
          if(extentDiffLeft<extentDiffRight)
            cur = &(*cur)->_left;
          else
            cur = &(*cur)->_right;
        }
        *cur = new Node((*cur)->_parent,*cur,node);
        sync(*cur);
      } else _root = node;
      return node;
    }
    void remove(Node* node){
      L_ASSERT(node->leaf());
      if(_root == node)
        _root = nullptr;
      else { // Has parent
        Node* parent(node->_parent);
        Node* sibling(node->sibling());
        if(_root == parent){
          _root = sibling;
          sibling->_parent = nullptr;
        } else // Has grand-parent
          parent->_parent->replace(parent,sibling);
        parent->_left = parent->_right = nullptr;
        delete parent;
        sync(sibling);
      }
      delete node;
    }
    Node* update(Node* node,const Key& key){ // TODO
      const V value(node->value());
      remove(node);
      return insert(key,value);
    }
    void query(const Key& zone,Array<Node*>& nodes){
      nodes.clear();
      if(_root && zone.overlaps(_root->_key))
        query(_root,zone,nodes);
    }
    void query(Node* node,const Key& zone,Array<Node*>& nodes){
      if(node->branch()){ // Branch
        if(zone.overlaps(node->_left->_key)) query(node->_left,zone,nodes);
        if(zone.overlaps(node->_right->_key)) query(node->_right,zone,nodes);
      } else nodes.push(node); // Leaf
    }
    void collisions(Array<Node*>& pairs){
      pairs.clear();
      if(_root && _root->branch()){
        uncross(_root);
        collisions(_root->_left,_root->_right,pairs);
      }
    }
    void collisions(Node* a,Node* b,Array<Node*>& pairs,bool crossed = false){
      const bool colliding(a->_key.overlaps(b->_key));
      if(colliding){
        if(a->leaf()){
          if(b->leaf()){
            pairs.push(a);
            pairs.push(b);
          } else {
            collisions(a,b->_left,pairs);
            collisions(a,b->_right,pairs);
          }
        } else {
          if(b->leaf()){
            collisions(a->_left,b,pairs);
            collisions(a->_right,b,pairs);
          } else {
            collisions(a->_left,b->_left,pairs);
            collisions(a->_left,b->_right,pairs);
            collisions(a->_right,b->_left,pairs);
            collisions(a->_right,b->_right,pairs);
          }
        }
      }
      if(a->branch() && !a->_crossed){
        a->_crossed = true;
        collisions(a->_left,a->_right,pairs,true);
      }
      if(b->branch() && !b->_crossed){
        b->_crossed = true;
        collisions(b->_left,b->_right,pairs,true);
      }
    }
    static void uncross(Node* node){
      node->_crossed = false;
      if(node->branch()){
        uncross(node->_left);
        uncross(node->_right);
      }
    }
    void sync(Node* node){
      if(node->leaf())
        node = node->_parent;
      while(node){
        node->_height = 1+max(node->_left->_height,node->_right->_height);
        node->_key = node->_left->_key+node->_right->_key;
        node = node->_parent;
      }
    }
    inline void print(){ if(_root) print(_root); }
    void print(Node* node){
      static int level(0);
      for(int i(0); i<level; i++)
        out << "  ";
      if(node->branch()){
        out << node->_key << ":\n";
        level++;
        print(node->_left);
        print(node->_right);
        level--;
      } else
        out << node->_key << ": " << node->_value << '\n';
    }
  };

  // Regular trees
  template <class V> class Interval2iTree : public IntervalTree<2,int,V> {};
  template <class V> class Interval2fTree : public IntervalTree<2,float,V> {};
  template <class V> class Interval3fTree : public IntervalTree<3,float,V> {};
}
