#include "bytes.h"

#include "../Exception.h"
#include "../macros.h"

using namespace L;

Vector<byte> L::bytesFromFile(String filePath) {
  std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::in);
  Vector<byte> wtr;
  if(file) {
    // get length of file:
    file.seekg(0,std::ios::end);
    wtr.resize(file.tellg());
    file.seekg(0,std::ios::beg);
    // read data as a block:
    file.read((char*)&wtr[0],wtr.size());
  } else throw Exception("Tried to open a file that didn't exist: " + filePath);
  return wtr;
}
void L::fileFromBytes(String filePath, const Vector<byte>& bytes) {
  std::ofstream file(filePath.c_str(), std::ios::binary | std::ios::out);
  if(file) {
    file.write((char*)&bytes[0],bytes.size());
    file.close();
  } else throw Exception("Couldn't create file: " + filePath);
}

uint L::btui(const Vector<byte>& bytes) {
  uint wtr(0);
  L_Iter(bytes,it) {
    wtr <<= 8;
    wtr |= (*it);
  }
  return wtr;
}
int L::bti(const Vector<byte>& bytes) {
  union {
    uint u;
    int i;
  } wtr;
  wtr.u = (bytes[0]&0x80) ? -1 : 0;
  L_Iter(bytes,it) {
    wtr.u <<= 8;
    wtr.u |= (*it);
  }
  return wtr.i;
}
float L::btf(const Vector<byte>& bytes) {
  return (float)bti(bytes)/(float)0x10000;
}
String L::bts(const Vector<byte>& bytes) {
  return String(bytes.begin(),bytes.end());
}

Vector<byte> L::uitb(uint n, uint size) {
  Vector<byte> wtr(size,0);
  while(size--) {
    wtr[size] = (n&0xFF);
    n>>=8;
  }
  return wtr;
}
Vector<byte> L::itb(int n, uint size) {
  Vector<byte> wtr(size,0);
  while(size--) {
    wtr[size] = (n&0xFF);
    n>>=8;
  }
  return wtr;
}
Vector<byte> L::ftb(float n) {
  return itb(n*(float)0x10000,4);
}
Vector<byte> L::stb(const String& s) {
  return Vector<byte>(s.begin(),s.end());
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
Vector<byte> L::htb(const String& s) {
  Vector<byte> wtr(s.size()/2);
  for(uint i(0); i<wtr.size(); i++)
    wtr[i] = (hex(s[i*2]) << 4) | hex(s[i*2+1]);
  return wtr;
}

ulong L::btulBE(Vector<byte> bytes) {
  ulong wtr(0), p(1);
  for(uint i=0; i<bytes.size(); i++) {
    wtr += bytes[i]*p;
    p <<= 8;
  }
  return wtr;
}
Vector<byte> L::ultbBE(ulong n, uint size) {
  Vector<byte> wtr(size);
  byte* b = &wtr.back();
  while(n) {
    *b-- = (n&0xFF);
    n >>= 8;
  }
  return wtr;
}
ulong L::btulLE(Vector<byte> bytes) {
  ulong wtr = 0, p = 0x1000000;
  for(uint i=0; i<bytes.size(); i++) {
    wtr += bytes[i]*p;
    p >>= 8;
  }
  return wtr;
}
Vector<byte> L::ultbLE(ulong n, uint size) {
  Vector<byte> wtr(size);
  byte* b = &wtr.front();
  while(n) {
    *b++ = (n&0xFF);
    n >>= 8;
  }
  return wtr;
}

std::ostream& L::operator<<(std::ostream &stream, const Vector<byte>& v) {
  stream.write((char*)&v[0],v.size());
  return stream;
}
