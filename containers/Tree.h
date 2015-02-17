#ifndef DEF_L_Tree
#define DEF_L_Tree

#include "../geometry/Point.h"
#include "../tmp.h"

namespace L{
    template <uint d,class K,class V>
    class Tree{
        protected:
            static const uint n = static_pow<2,d>::value;
            typedef Point<d,K> key_t;
            class Node{
                protected:
                    V* value;
                    key_t key;
                    Node* children[n];
                public:
                    Node(const key_t& key, V* value) : key(key), value(value){
                        for(uint i(0);i<n;i++)  children[i] = NULL;
                    }
                    ~Node(){
                        for(uint i(0);i<n;i++)
                            delete children;
                        delete value;
                    }
                    Node* insert(const key_t& key, V* value){
                        uint child(cmp(this->key,key));
                        return (child!=-1) ? insert(children[child],key,value) : this;
                    }
                    Node* find(const key_t& key){
                        uint child(cmp(this->key,key));
                        return (child!=-1) ? find(children[child],key) : this;
                    }
                    uint height() const{
                        uint wtr(1);
                        for(uint i(0);i<n;i++)
                            wtr = std::max(wtr,1+height(children[i]));
                        return wtr;
                    }
                    static uint cmp(const key_t& a, const key_t& b){
                        uint eq(0), wtr(0);
                        for(uint i(0);i<d;i++){
                            wtr <<= 1;
                            if(a[i]>b[i])       wtr++;
                            else if(a[i]==b[i]) eq++;
                        }
                        return (eq<d) ? wtr : -1;
                    }
                    static Node* insert(Node*& node, const key_t& key, V* value){
                        if(node){
                            Node* wtr(node->insert(key,value));
                            // Self-balancing

                            return wtr;
                        }
                        else return node = new Node(key,value); // It's a leaf
                    }
                    static Node* find(Node* node, const key_t& key){
                        return (node) ? node->find(key) : NULL;
                    }
                    static uint height(Node* node){
                        return (node) ? node->height() : 0;
                    }
            };
            Node* root;

        public:
            Tree() : root(NULL){}
            ~Tree(){delete root;}
            inline Node* insert(const key_t& key, V* value = NULL){
                return Node::insert(root,key,value);
            }
            inline Node* insert(const key_t& key, const V& value){
                return Node::insert(root,key,new V(value));
            }
            inline Node* find(const key_t& key) const{
                return Node::find(root,key);
            }
            inline uint height() const{
                return Node::height(root);
            }

    };
    // Regular trees
    template <class K,class V> class BinaryTree : public Tree<1,K,V>{};
    template <class K,class V> class QuadTree : public Tree<2,K,V>{};
    template <class K,class V> class Octree : public Tree<3,K,V>{};
}

#endif



