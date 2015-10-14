#include "bytes.h"

#include "../Exception.h"
#include "../macros.h"

using namespace L;

Array<byte> L::bytesFromFile(const String& filePath) {
  std::ifstream file(filePath, std::ios::binary | std::ios::in);
  Array<byte> wtr;
  if(file) {
    // get length of file:
    file.seekg(0,std::ios::end);
    wtr.size(file.tellg());
    file.seekg(0,std::ios::beg);
    // read data as a block:
    file.read((char*)&wtr[0],wtr.size());
  } else throw Exception("Tried to open a file that didn't exist: " + filePath);
  return wtr;
}
void L::fileFromBytes(const String& filePath, const Array<byte>& bytes) {
  std::ofstream file(filePath, std::ios::binary | std::ios::out);
  if(file) {
    file.write((char*)&bytes[0],bytes.size());
    file.close();
  } else throw Exception("Couldn't create file: " + filePath);
}

uint L::btui(const Array<byte>& bytes) {
  uint wtr(0);
  for(int i(0); i<bytes.size(); i++) {
    wtr <<= 8;
    wtr |= bytes[i];
  }
  return wtr;
}
int L::bti(const Array<byte>& bytes) {
  union {
    uint u;
    int i;
  } wtr;
  wtr.u = (bytes[0]&0x80) ? -1 : 0;
  for(int i(0); i<bytes.size(); i++) {
    wtr.u <<= 8;
    wtr.u |= bytes[i];
  }
  return wtr.i;
}
float L::btf(const Array<byte>& bytes) {
  return (float)bti(bytes)/(float)0x10000;
}
String L::bts(const Array<byte>& bytes) {
  return String((const char*)&bytes[0],bytes.size());
}

Array<byte> L::uitb(uint n, uint size) {
  Array<byte> wtr(size,0);
  while(size--) {
    wtr[size] = (n&0xFF);
    n>>=8;
  }
  return wtr;
}
Array<byte> L::itb(int n, uint size) {
  Array<byte> wtr(size,0);
  while(size--) {
    wtr[size] = (n&0xFF);
    n>>=8;
  }
  return wtr;
}
Array<byte> L::ftb(float n) {
  return itb(n*(float)0x10000,4);
}
Array<byte> L::stb(const String& s) {
  return Array<byte>((const byte*)(const char*)s,s.size());
}
byte hex(char c) {
  if(c>='0'&&c<='9')
    return c-'0';
  else if(c>='A'&&c<='F')
    return c-'A'+10;
  else if(c>='a'&&c<='f')
    return c-'a'+10;
  throw Exception(c+" is not an hexadecimal character.");
}
Array<byte> L::htb(const String& s) {
  Array<byte> wtr(s.size()/2,0);
  for(uint i(0); i<wtr.size(); i++)
    wtr[i] = (hex(s[i*2]) << 4) | hex(s[i*2+1]);
  return wtr;
}

ulong L::btulBE(Array<byte> bytes) {
  ulong wtr(0), p(1);
  for(uint i=0; i<bytes.size(); i++) {
    wtr += bytes[i]*p;
    p <<= 8;
  }
  return wtr;
}
Array<byte> L::ultbBE(ulong n, uint size) {
  Array<byte> wtr(size,0);
  byte* b = &wtr.back();
  while(n) {
    *b-- = (n&0xFF);
    n >>= 8;
  }
  return wtr;
}
ulong L::btulLE(Array<byte> bytes) {
  ulong wtr = 0, p = 0x1000000;
  for(uint i=0; i<bytes.size(); i++) {
    wtr += bytes[i]*p;
    p >>= 8;
  }
  return wtr;
}
Array<byte> L::ultbLE(ulong n, uint size) {
  Array<byte> wtr(size,0);
  byte* b = &wtr[0];
  while(n) {
    *b++ = (n&0xFF);
    n >>= 8;
  }
  return wtr;
}

std::ostream& L::operator<<(std::ostream &stream, const Array<byte>& v) {
  stream.write((char*)&v[0],v.size());
  return stream;
}
