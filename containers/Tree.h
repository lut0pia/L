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

          int childIndex(const Key& key) const { // Returns the child index for a key, returns -1 when both keys are equal
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
          inline static int mirrorIndex(int index, int components) {
            return (index^components)&mask;
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
          static void nearest(const Node* node, const Node*& bestNode, K& bestDistance, const Key& key) {
            if(node) {
              K distance(node->key().distSquared(key));
              if(distance<bestDistance) {
                bestNode = node;
                bestDistance = distance;
              }
              int bestChild(node->childIndex(key));
              for(int i(0); i<n; i++) { // For each possible combination of component mirroring
                Key nearestPossibleVector(key-node->key()); // Compute minimum vector coming from mirrored best child
                for(int j(0); j<d; j++)
                  if(!(i&(1<<j)))
                    nearestPossibleVector[(d-1)-j] = 0;
                K nearestPossibleDistance(nearestPossibleVector.normSquared()); // Compute minimum distance from mirrored best child
                if(bestDistance>=nearestPossibleDistance) // Check if mirrored best child could return a node nearer than the current one
                  nearest(node->_children[mirrorIndex(bestChild,i)],bestNode,bestDistance,key);
              }
            }
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
          static int size(const Node* node) {
            if(node) {
              int wtr(1);
              for(int i(0); i<n; i++)
                wtr += size(node->_children[i]);
              return wtr;
            } else return 0;
          }
      };
      Node* _root;

    public:
      Tree() : _root(NULL) {}
      ~Tree() {delete _root;}
      inline void insert(const Key& key, const V& value) {
        Node::insert(_root,key,value);
        //std::cout << balance() << std::endl;
      }
      inline const Node* find(const Key& key) const {
        return Node::find(_root,key);
      }
      inline const Node* nearest(const Key& key, const K& maxDistance = std::numeric_limits<K>::max()) const {
        const Node* best(NULL);
        K distance(maxDistance*maxDistance); // All distances are squared in the algorithm
        Node::nearest(_root,best,distance,key);
        return best;
      }
      inline int height() const {
        return Node::height(_root);
      }
      inline int size() const {
        return Node::size(_root);
      }
      float balance() const {
        return (log(size())/log(n))/height();
      }
  };
  // Regular trees
  template <class K,class V> class BinaryTree : public Tree<1,K,V> {};
  template <class K,class V> class QuadTree : public Tree<2,K,V> {};
  template <class K,class V> class Octree : public Tree<3,K,V> {};
}

#endif



