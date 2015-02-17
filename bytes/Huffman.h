#ifndef DEF_L_Huffman
#define DEF_L_Huffman

#include "../containers/Ref.h"
#include "../macros.h"
#include "../stl.h"

namespace L{
    class Huffman{
        public:
            class Tree{
                private:
                    size_t weight;
                    Set<byte> bytes;
                    Ref<Tree> zero, one;
                public:
                    bool operator<(const Tree&) const;

                    void write(byte, Vector<byte>&, size_t& bit) const;
                    byte read(const Vector<byte>&, size_t& bit) const;
                friend class Huffman;
            };
            static Tree makeTree(size_t weight[256]);
            static Tree makeTree(const Vector<byte>&);
            static Vector<byte> encode(const Vector<byte>&, const Tree&);
            static Vector<byte> decode(const Vector<byte>&, const Tree&, size_t max = -1);
    };
}

#endif






