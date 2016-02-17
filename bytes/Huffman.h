#ifndef DEF_L_Huffman
#define DEF_L_Huffman

#include "../containers/Ref.h"
#include "../macros.h"
#include "../types.h"
#include "../containers/Array.h"
#include "../containers/Set.h"

namespace L {
  class Huffman {
    public:
      class Tree {
        private:
          size_t weight;
          Set<byte> bytes;
          Ref<Tree> zero, one;
        public:
          bool operator<(const Tree&) const;
          bool operator>(const Tree&) const;

          void write(byte, Array<byte>&, size_t& bit) const;
          byte read(const Array<byte>&, size_t& bit) const;
          friend class Huffman;
      };
      static Tree makeTree(size_t weight[256]);
      static Tree makeTree(const Array<byte>&);
      static Array<byte> encode(const Array<byte>&, const Tree&);
      static Array<byte> decode(const Array<byte>&, const Tree&, size_t max = -1);
  };
}

#endif






