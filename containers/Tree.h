#ifndef DEF_L_Tree
#define DEF_L_Tree

#include "../geometry/Vector.h"
#include "../tmp.h"
#include "Pool.h"

namespace L {
  template <int d,class K,class V>
  class Tree {
    private:
      static const int n = static_pow<2,d>::value;
      static const int mask = ~((~0)<<d);
      typedef Vector<d,K> Key;
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

          static void insert(Node*& node, const Key& key, const V& value) {
            if(node) {
              int child(node->childIndex(key));
              if(child>=0) insert(node->_children[child],key,value);
              else node->_value = value;
            } else node = new Node(key,value); // It's a leaf
          }
          static const Node* find(const Node* node, const Key& key) {
            if(node) {
              int child(node->childIndex(key));
              return (child>=0) ? find(node->_children[child],key) : node;
            } else return NULL;
          }
          static void nearest(const Node* node, const Node*& bestNode, K& bestDistance, const Key& target) {
            if(node) {
              Key keyToTarget(target-node->key());
              K distance(keyToTarget.normSquared());
              if(distance<bestDistance) {
                bestNode = node;
                bestDistance = distance;
              }
              int bestChild(node->childIndex(target));
              for(int i(0); i<n; i++) { // For each possible combination of component mirroring
                K nearestPossibleDistance(0); // Compute nearest possible distance by only taking some axes into account
                for(int j(0); j<d; j++)
                  if(i&(n>>(j+1)))
                    nearestPossibleDistance += keyToTarget[j]*keyToTarget[j];
                if(bestDistance>nearestPossibleDistance) // Check if mirrored best child could return a node nearer than the current one
                  nearest(node->_children[mirrorIndex(bestChild,i)],bestNode,bestDistance,target);
              }
            }
          }
          static void construct(Node*& node, const Vector<d,K>& center, const Array<Node*>& nodes) {
            if(!nodes.empty()) {
              K distance(std::numeric_limits<K>::max());
              for(int i(0); i<nodes.size(); i++) { // Find node closest to the center of the current interval
                K tmp(center.distSquared(nodes[i]->key()));
                if(tmp<distance) {
                  node = nodes[i];
                  distance = tmp;
                }
              }
              for(int i(0); i<n; i++) { // Cycle through all children
                Vector<d,K> childCenter(0);
                Array<Node*> childNodes;
                for(int j(0); j<nodes.size(); j++)
                  if(node->childIndex(nodes[j]->key())==i) { // Find nodes that should go to that child
                    childNodes.push(nodes[j]);
                    childCenter += nodes[j]->key(); // Compute the average of that child's nodes
                  }
                if(!childNodes.empty())
                  childCenter /= childNodes.size();
                construct(node->_children[i],childCenter,childNodes);
              }
            }
          }
          static void destruct(Node* node, Array<Node*>& nodes) { // This method collects all nodes in a list and removes all links between them
            if(node) {
              nodes.push(node); // Collect this now
              for(int i(0); i<n; i++) {
                destruct(node->_children[i],nodes); // Go through children
                node->_children[i] = NULL; // Remove all links
              }
            }
          }
          static void balance(Node*& node) {
            Array<Node*> nodes;
            Vector<d,K> center(0);
            destruct(node,nodes); // Start by collecting all nodes
            for(int i(0); i<nodes.size(); i++)
              center += nodes[i]->key();
            if(!nodes.empty())
              center /= nodes.size();
            construct(node,center,nodes); // Feed all nodes back inside the tree
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

          inline void* operator new(size_t size) {
            return Pool<Node>::global.allocate();
          }
          inline void operator delete(void* p) {
            Pool<Node>::global.deallocate(p);
          }
      };
      Node* _root;

    public:
      Tree() : _root(NULL) {}
      ~Tree() {delete _root;}
      inline void insert(const Key& key, const V& value) {
        Node::insert(_root,key,value);
      }
      inline const Node* find(const Key& key) const {
        return Node::find(_root,key);
      }
      inline const Node* nearest(const Key& key, const K& maxDistance) const {
        const Node* best(NULL);
        K distance(maxDistance*maxDistance); // All distances are squared in the algorithm
        Node::nearest(_root,best,distance,key);
        return best;
      }
      inline const Node* nearest(const Key& key) const {
        const Node* best(NULL);
        K distance(std::numeric_limits<K>::max());
        Node::nearest(_root,best,distance,key);
        return best;
      }
      inline void balance() {
        Node::balance(_root);
      }
      inline int height() const {
        return Node::height(_root);
      }
      inline int size() const {
        return Node::size(_root);
      }
  };

  // Regular trees
  template <class K,class V> class BinaryTree : public Tree<1,K,V> {};
  template <class K,class V> class QuadTree : public Tree<2,K,V> {};
  template <class K,class V> class Octree : public Tree<3,K,V> {};
}

#endif



