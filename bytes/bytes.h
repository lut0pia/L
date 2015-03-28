#ifndef DEF_L_bytes
#define DEF_L_bytes

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include "../types.h"
#include "../stl/String.h"
#include "../stl/Vector.h"

namespace L{
    Vector<byte> bytesFromFile(String filePath); // Returns a Vector containing every byte in a file
    void fileFromBytes(String filePath, const Vector<byte>&); // Creates a file with the byte Vector

    // These functions are made to read and write FOR each other, don't forget that
    uint btui(const Vector<byte>&);
    int bti(const Vector<byte>&);
    float btf(const Vector<byte>&);
    String bts(const Vector<byte>&);

    Vector<byte> uitb(uint, uint size = sizeof(uint));
    Vector<byte> itb(int, uint size = sizeof(int));
    Vector<byte> ftb(float);
    Vector<byte> stb(const String&);
    Vector<byte> htb(const String&);

    // Now these are more specific
    ulong btulBE(Vector<byte>);
    Vector<byte> ultbBE(ulong, uint size = sizeof(ulong));
    ulong btulLE(Vector<byte>);
    Vector<byte> ultbLE(ulong, uint size = sizeof(ulong));

    std::ostream& operator<<(std::ostream&, const Vector<byte>&);

    template <class T>
    String Binary(T a){ // Will return a String with the binary of basic types
        String wtr(sizeof(T)*8,'0');
        size_t i = wtr.size()-1;
        char* c = (char*)&a, tmp;
        while(c<(char*)&a+sizeof(T)){
            tmp = 1;
            while(tmp){
                if(*c&tmp)
                    wtr[i] = '1';
                tmp <<= 1;
                i--;
            }
            c++;
        }
        return wtr;
    }
    template <class T>
    String Hexadecimal(T a){ // Will return a String with the hex of basic types
        String wtr(sizeof(T)*2,'0');
        size_t i = wtr.size()-1;
        byte* c = (byte*)&a;
        while(c<(byte*)(&a)+sizeof(T)){
            int tmp = (*c&0xF);
            wtr[i--] = (tmp < 10) ? tmp+'0' : tmp+'A'-10;
            tmp = (*c&0xF0)>>4;
            wtr[i--] = (tmp < 10) ? tmp+'0' : tmp+'A'-10;
            c++;
        }
        return wtr;
    }
}

#endif

