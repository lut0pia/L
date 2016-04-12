#include "Huffman.h"

using namespace L;

bool Huffman::Tree::operator<(const Tree& other) const {
  return weight < other.weight;
}
bool Huffman::Tree::operator>(const Tree& other) const {
  return weight > other.weight;
}
void Huffman::Tree::write(byte b, Array<byte>& v, size_t& bit) const {
  if(v.size()<=bit/8)
    v.size((bit/8)+1);
  if(bytes.size()>1) {
    if(zero != nullptr && zero->bytes.has(b))
      zero->write(b,v,++bit);
    else {
      v[bit/8] |= (1 << (7-bit%8));
      one->write(b,v,++bit);
    }
  }
}
byte Huffman::Tree::read(const Array<byte>& v, size_t& bit) const {
  if(bytes.size()==1)
    return bytes[0];
  else {
    if((v[bit/8]>>(7-bit%8))&1)
      return one->read(v,++bit);
    else
      return zero->read(v,++bit);
  }
}

Huffman::Tree Huffman::makeTree(size_t weight[256]) {
  SortedArray<Tree> trees;
  Tree tmp;
  for(size_t i=0; i<256; i++) { // Initialize ordered container
    tmp.weight = weight[i];
    tmp.bytes.clear();
    tmp.bytes.insert(i);
    trees.insert(tmp);
  }
  while(trees.size()>1) {
    tmp.zero = new Tree(trees[0]);
    trees.erase(0);
    tmp.one = new Tree(trees[0]);
    trees.erase(0);
    tmp.weight = tmp.zero->weight + tmp.one->weight;
    tmp.bytes = tmp.zero->bytes + tmp.one->bytes;
    trees.insert(tmp);
  }
  return trees[0];
}
Huffman::Tree Huffman::makeTree(const Array<byte>& v) {
  size_t weight[256] = {0};
  for(auto&& b : v)
    weight[b]++;
  return makeTree(weight);
}
Array<byte> Huffman::encode(const Array<byte>& v, const Tree& t) {
  Array<byte> wtr(1,0);
  size_t bit(0);
  for(auto&& b : v)
    t.write(b,wtr,bit);
  return wtr;
}
Array<byte> Huffman::decode(const Array<byte>& v, const Tree& t, size_t max) {
  Array<byte> wtr;
  size_t bit(0);
  while(max-- && bit<v.size()*8)
    wtr.push(t.read(v,bit));
  return wtr;
}
