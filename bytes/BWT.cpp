#include "BWT.h"

#include <algorithm>
#include <utility>

using namespace L;

bool cmp(const std::pair<const Vector<byte>*,size_t>& a, const std::pair<const Vector<byte>*,size_t>& b){
    const Vector<byte>& bytes = (*a.first);
    size_t size = bytes.size();

    for(size_t i=0;i<size;i++){
        if(bytes[(a.second+i)%size] < bytes[(b.second+i)%size])         // Lesser
            return true;
        else if(bytes[(a.second+i)%size] > bytes[(b.second+i)%size])    // Greater
            return false;
    }
    return true;
}

Vector<byte> BWT::encode(const Vector<byte>& bytes, size_t& index){
    size_t size = bytes.size();
    Vector<std::pair<const Vector<byte>*,size_t> > rotations(size);
    Vector<byte> wtr(size);
    for(size_t i=0;i<size;i++) rotations[i] = make_pair(&bytes,i);
    std::sort(rotations.begin(),rotations.end(),cmp); // Sort
    for(size_t i=0;i<size;i++){ // Take last column
        if(rotations[i].second==0) index = i;  // Store index
        wtr[i] = bytes[(rotations[i].second+size-1)%size];
    }
    return wtr;
}
Vector<byte> BWT::decode(const Vector<byte>& bytes, size_t index){
    size_t size = bytes.size();
    Vector<byte> wtr(size), sorted(bytes);
    sort(sorted.begin(),sorted.end());
    for(size_t i=0;i<size;i++){
        char c = wtr[i] = sorted[index];
        size_t count = 1;
        for(size_t j=0;j<index;j++) // Find position of letter in sorted
            if(sorted[j]==c)
                count++;
        for(index=0;count>0;index++) // Seek letter in encoded String
            if(bytes[index]==c)
                count--;
        index--;
    }
    return wtr;
}
