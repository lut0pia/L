#ifndef DEF_L_Tree
#define DEF_L_Tree

#include "../geometry/Point.h"
#include "../tmp.h"

namespace L {
  template <int d,class K,class V>
  class Tree {
    protected:
      static const int n = static_pow<2,d>::value;
      static const int mask = ~((~0)<<d);
      typedef Point<d,K> Key;
      class Node {
        private:
          V _value;
          Key _key;
          Node* _children[n];
        public:
          Node(const Key& key, const V& value) : _key(key), _value(value) {
            for(int i(0); i<n; i++)
              _children[i] = NULL;
          }
          ~Node() {
            for(int i(0); i<n; i++)
              delete _children[i];
          }
          inline const Key& key() const {return _key;}
          inline const V& value() const {return _value;}

          int childIndex(const Key& key) const { // Returns the child index, returns -1 when both keys are equal
            int eq(0), wtr(0);
            for(int i(0); i<d; i++) {
              wtr <<= 1;
              if(_key[i]>key[i])       wtr++;
              else if(_key[i]==key[i]) eq++;
            }
            return (eq<d) ? wtr : -1;
          }
          inline static int oppositeIndex(int index) {
            return (~index)&mask;
          }

          static Node* insert(Node*& node, const Key& key, const V& value) {
            if(node) {
              int child(node->childIndex(key));
              if(child>=0) {
                Node* wtr(insert(node->_children[child],key,value));
                // Self-balancing
                return wtr;
              } else node->_value = value;
            } else return node = new Node(key,value); // It's a leaf
          }
          static const Node* find(const Node* node, const Key& key) {
            if(node) {
              int child(node->childIndex(key));
              return (child>=0) ? find(node->_children[child],key) : node;
            } else return NULL;
          }
          static const Node* nearest(const Node* node, const Key& key) {
            if(node) {
              K distance(node->key().distSquared(key));
              int child(node->childIndex(key));
              if(child<0) return node; // This node has this key (so it's nearest)
              int oppositeChild(oppositeIndex(child));
              const Node* wtr(node);
              for(int i(0); i<n; i++)
                if(i!=oppositeChild) {
                  const Node* tmpNode(nearest(node->_children[i],key));
                  if(tmpNode) {
                    K tmpDistance(tmpNode->key().distSquared(key));
                    //std::cout << tmpNode->key() << " ->" << tmpDistance << " (" << distance << ")" << std::endl;
                    if(tmpDistance<distance) {
                      wtr = tmpNode;
                      distance = tmpDistance;
                    }
                  }
                }
              return wtr;
            } else return NULL;
          }
          static int height(const Node* node) {
            if(node) {
              int wtr(1);
              for(int i(0); i<n; i++) {
                int tmp(1+height(node->_children[i]));
                if(wtr<tmp)
                  wtr = tmp;
              }
              return wtr;
            } else return 0;
          }
      };
      Node* _root;

    public:
      Tree() : _root(NULL) {}
      ~Tree() {delete _root;}
      inline Node* insert(const Key& key, const V& value) {
        return Node::insert(_root,key,value);
      }
      inline const Node* find(const Key& key) const {
        return Node::find(_root,key);
      }
      inline const Node* nearest(const Key& key) const {
        return Node::nearest(_root,key);
      }
      inline int height() const {
        return Node::height(_root);
      }
  };
  // Regular trees
  template <class K,class V> class BinaryTree : public Tree<1,K,V> {};
  template <class K,class V> class QuadTree : public Tree<2,K,V> {};
  template <class K,class V> class Octree : public Tree<3,K,V> {};
}

#endif



