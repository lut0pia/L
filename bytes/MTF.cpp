#include "MTF.h"

using namespace L;

Vector<byte> MTF::encode(const Vector<byte>& bytes){
    size_t size = bytes.size();
    Vector<byte> wtr(size);
    byte t[256];
    for(size_t i=0;i<256;i++) t[i] = i;
    for(size_t i=0;i<size;i++){
        byte b = bytes[i];
        wtr[i] = t[b];
        if(t[b]!=0){ // Need to move the index
            for(size_t j=0;j<256;j++)
                if(t[j]<t[b]) t[j]++;
            t[b] = 0;
        }
    }
    return wtr;
}
Vector<byte> MTF::decode(const Vector<byte>& bytes){
    size_t size = bytes.size();
    Vector<byte> wtr(size);
    byte t[256];
    for(size_t i=0;i<256;i++) t[i] = i;
    for(size_t i=0;i<size;i++){
        byte b = bytes[i];
        wtr[i] = t[b];
        for(size_t j=b;j>0;j--)
            t[j] = t[j-1];
        t[0] = wtr[i];
    }
    return wtr;
}
