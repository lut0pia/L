#ifndef DEF_L_BTree
#define DEF_L_BTree

namespace L {
  template <size_t d,class T>
  class BTree {
    protected:
      size_t size; // Number of used elements
      T elements[d*2];
      BTree<d,T>* children[(d*2)+1];
    public:
      BTree() {
        size = 0;
        for(size_t i(0); i<d; i++)
          children[i] = nullptr;
      }
      bool find(const T& e) {
        return false;
      }
      void add(const T& e) {
        if(!size)
          elements[0] = e;
        size++;
      }

  };
}

#endif



