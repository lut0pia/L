#pragma once

#include "../math/Interval.h"
#include "Pool.h"

namespace L {
  template <int d, class K, class V>
  class IntervalTree {
  public:
    typedef Interval<d, K> Key;
    class Node {
    private:
      Node *_parent, *_left, *_right;
      Key _key;
      V _value;
    public:
      inline Node(const Key& key, const V& value) : _parent(nullptr), _left(nullptr), _key(key), _value(value) {}
      inline Node(Node* parent, Node* oldNode, Node* newNode) : _parent(parent), _left(oldNode), _right(newNode) {
        if(parent)
          parent->replace(oldNode, this);
        _left->_parent = _right->_parent = this;
      }
      inline ~Node() {
        if(branch()) {
          delete _left;
          delete _right;
        }
      }
      inline const Key& key() const { return _key; }
      inline const V& value() const { return _value; }
      inline const Node* left() const { return _left; }
      inline const Node* right() const { return _right; }
      inline bool branch() const { return _left!=nullptr; }
      inline bool leaf() const { return _left==nullptr; }
      inline Node* childNot(Node* node) { return (_left!=node) ? _left : _right; }
      inline Node* sibling() { return _parent->childNot(this); }
      inline void replace(Node* oldNode, Node* newNode) {
        ((_left==oldNode) ? _left : _right) = newNode;
        newNode->_parent = this;
      }
      inline void refit() {
        if(branch())
          _key = _left->_key+_right->_key;
      }

      inline void* operator new(size_t size) { return Pool<Node>::global.allocate(); }
      inline void operator delete(void* p) { Pool<Node>::global.deallocate(p); }
      friend IntervalTree;
    };
  private:
    Node* _root;
  public:
    constexpr IntervalTree() : _root(nullptr) {}
    inline ~IntervalTree() { delete _root; }
    inline const Node* root() const { return _root; }
    Node* insert(const Key& key, const V& value) {
      Node* node(new Node(key, value));
      if(_root) {
        Node** cur(&_root);
        while((*cur)->branch()) {
          const Node *left((*cur)->_left), *right((*cur)->_right);
          const Key& leftKey(left->_key), rightKey(right->_key);
          const Key newLeft(leftKey+key), newRight(rightKey+key);
          const float extentDiffLeft(newLeft.extent()-leftKey.extent()),
            extentDiffRight(newRight.extent()-rightKey.extent());
          cur = (extentDiffLeft<extentDiffRight) ? (&(*cur)->_left) : (&(*cur)->_right);
        }
        *cur = new Node((*cur)->_parent, *cur, node);
        sync(*cur);
      } else _root = node;
      return node;
    }
    void remove(Node* node) {
      L_ASSERT(node->leaf());
      if(_root == node)
        _root = nullptr;
      else { // Has parent
        Node* parent(node->_parent);
        Node* sibling(node->sibling());
        if(_root == parent) {
          _root = sibling;
          sibling->_parent = nullptr;
        } else // Has grand-parent
          parent->_parent->replace(parent, sibling);
        parent->_left = parent->_right = nullptr;
        delete parent;
        sync(sibling);
      }
      delete node;
    }
    void update(Node* node, const Key& key) {
      node->_key = key;
      sync(node);
    }
    inline void query(const Key& zone, Array<Node*>& nodes) {
      nodes.clear();
      if(_root && zone.overlaps(_root->_key))
        query(_root, zone, nodes);
    }
    static void query(Node* node, const Key& zone, Array<Node*>& nodes) {
      if(node->branch()) { // Branch
        if(zone.overlaps(node->_left->_key)) query(node->_left, zone, nodes);
        if(zone.overlaps(node->_right->_key)) query(node->_right, zone, nodes);
      } else nodes.push(node); // Leaf
    }
    inline bool overlaps(const Key& zone) const {
      return _root && _root->_key.overlaps(zone) && overlaps(_root, zone);
    }
    static bool overlaps(Node* node, const Key& zone) {
      if(node->branch()) { // Branch
        return (node->_left->_key.overlaps(zone) && overlaps(node->_left, zone))
          || (node->_right->_key.overlaps(zone) && overlaps(node->_right, zone));
      } else return node->_key.overlaps(zone); // Leaf
    }
    void collisions(Array<Node*>& pairs) {
      pairs.clear();
      if(_root && _root->branch()) {
        struct Pair { Node *a, *b; };
        StaticStack<1024, Pair> candidatePairs;
        candidatePairs.push(_root->_left, _root->_right);
        while(!candidatePairs.empty()) {
          Node* a(candidatePairs.top().a);
          Node* b(candidatePairs.top().b);
          candidatePairs.pop();
          const bool colliding(a->_key.overlaps(b->_key));
          if(colliding) {
            if(a->leaf()) {
              if(b->leaf()) {
                pairs.push(a);
                pairs.push(b);
              } else {
                candidatePairs.push(a, b->_left);
                candidatePairs.push(a, b->_right);
              }
            } else {
              if(b->leaf()) {
                candidatePairs.push(a->_left, b);
                candidatePairs.push(a->_right, b);
              } else {
                candidatePairs.push(a->_left, b->_left);
                candidatePairs.push(a->_left, b->_right);
                candidatePairs.push(a->_right, b->_left);
                candidatePairs.push(a->_right, b->_right);
              }
            }
          }
          if(a->sibling()==b) {
            if(a->branch())
              candidatePairs.push(a->_left, a->_right);
            if(b->branch())
              candidatePairs.push(b->_left, b->_right);
          }
        }
      }
    }
    static void sync(Node* node) {
      refit(node);
      balance(node);
    }
    static void refit(Node* node) {
      if(node->leaf())
        node = node->_parent;
      while(node) {
        node->refit();
        node = node->_parent;
      }
    }
    static inline float cost(const Key& a, const Key& b) {
      return abs(a.extent()-b.extent())+(a*b).extent();
    }
    static void balance(Node* node) {
      while(node && (!node->_left || !node->_right))
        node = node->_parent;
      while(node) {
        /*
              node
             /   \
            L     R
           / \   / \
          LL LR RL RR
        */
        Node *L(node->_left), *R(node->_right), *LL(L->_left), *LR(L->_right), *RL(R->_left), *RR(R->_right);
        Node *a(nullptr), *b(nullptr);
        float minCost(cost(L->_key, R->_key));
#define L_ROTATION_COST(A,B,AC,BC) {const float c(cost(AC,BC)); if(c<minCost){ minCost = c; a = A; b = B; } }
#define L_ROTATION_COST_GC_C(A,B) L_ROTATION_COST(A,B,A->sibling()->_key+B->_key,A->_key)
        if(L->branch()) {
          L_ROTATION_COST_GC_C(LL, R);
          L_ROTATION_COST_GC_C(LR, R);
        }
        if(R->branch()) {
          L_ROTATION_COST_GC_C(RL, L);
          L_ROTATION_COST_GC_C(RR, L);
        }
        /* GC to GC
        if(L->branch() && R->branch()){
          L_ROTATION_COST(LL,RR,RR->_key+LR->_key,LL->_key+RL->_key);
          L_ROTATION_COST(LL,RL,LL->_key+RR->_key,LR->_key+RL->_key);
        }*/
        if(a) swap(a, b); // We found a beneficial rotation
        node = node->_parent;
      }
    }
    static void swap(Node* a, Node* b) {
      L_ASSERT(a->_parent && b->_parent && a->_parent!=b->_parent);
      Node *aParent(a->_parent), *bParent(b->_parent);
      aParent->replace(a, b);
      bParent->replace(b, a);
      a->refit();
      b->refit();
      aParent->refit();
      bParent->refit();
    }
    inline void print() { if(_root) print(_root); }
    static void print(Node* node) {
      static int level(0);
      for(int i(0); i<level; i++)
        out << "  ";
      if(node->branch()) {
        out << node->_key << ": " << node->_height << "\n";
        level++;
        print(node->_left);
        print(node->_right);
        level--;
      } else
        out << node->_key << ": " << node->_value << '\n';
    }
    inline void draw() { if(_root) draw(_root); }
    static void draw(Node* node) {
      static int level(0);
      GL::baseColorProgram().use();
      GL::baseColorProgram().uniform("color", Color::fromHSV(pmod(level*15.f, 360.f), .5f, 1.f));
      GL::baseColorProgram().uniform("model", Matrix44f::translation(node->_key.center())*Matrix44f::scale(node->_key.size()*.5f));
      GL::wireCube().draw();
      if(node->branch()) {
        level++;
        draw(node->_left);
        draw(node->_right);
        level--;
      }
    }
  };

  // Regular trees
  template <class V> class Interval2iTree : public IntervalTree<2, int, V> {};
  template <class V> class Interval2fTree : public IntervalTree<2, float, V> {};
  template <class V> class Interval3fTree : public IntervalTree<3, float, V> {};
}
