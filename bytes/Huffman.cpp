#include "Huffman.h"

using namespace L;

bool Huffman::Tree::operator<(const Tree& other) const {
  return weight < other.weight;
}
void Huffman::Tree::write(byte b, Vector<byte>& v, size_t& bit) const {
  if(v.size()<=bit/8)
    v.resize((bit/8)+1);
  if(bytes.size()>1) {
    if(zero != NULL && zero->bytes.has(b))
      zero->write(b,v,++bit);
    else {
      v[bit/8] |= (1 << (7-bit%8));
      one->write(b,v,++bit);
    }
  }
}
byte Huffman::Tree::read(const Vector<byte>& v, size_t& bit) const {
  if(bytes.size()==1)
    return *bytes.begin();
  else {
    if((v[bit/8]>>(7-bit%8))&1)
      return one->read(v,++bit);
    else
      return zero->read(v,++bit);
  }
}

Huffman::Tree Huffman::makeTree(size_t weight[256]) {
  std::multiset<Tree> trees;
  Tree tmp;
  for(size_t i=0; i<256; i++) { // Initialize ordered container
    tmp.weight = weight[i];
    tmp.bytes.clear();
    tmp.bytes.insert(i);
    trees.insert(tmp);
  }
  while(trees.size()>1) {
    tmp.zero = new Tree(*trees.begin());
    trees.erase(trees.begin());
    tmp.one = new Tree(*trees.begin());
    trees.erase(trees.begin());
    tmp.weight = tmp.zero->weight + tmp.one->weight;
    tmp.bytes = tmp.zero->bytes + tmp.one->bytes;
    trees.insert(tmp);
  }
  return *trees.begin();
}
Huffman::Tree Huffman::makeTree(const Vector<byte>& v) {
  size_t weight[256] = {0};
  L_Iter(v,it) weight[*it]++;
  return makeTree(weight);
}
Vector<byte> Huffman::encode(const Vector<byte>& v, const Tree& t) {
  Vector<byte> wtr(1,0);
  size_t bit(0);
  L_Iter(v,it)
  t.write(*it,wtr,bit);
  return wtr;
}
Vector<byte> Huffman::decode(const Vector<byte>& v, const Tree& t, size_t max) {
  Vector<byte> wtr;
  size_t bit(0);
  while(max-- && bit<v.size()*8)
    wtr.push_back(t.read(v,bit));
  return wtr;
}
