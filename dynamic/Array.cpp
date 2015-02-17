#include "Array.h"

#include "../macros.h"

using namespace L;
using namespace Dynamic;

Array& Array::operator()(const Variable& other){
    push_back(other);
    return *this;
}
std::ostream& L::Dynamic::operator<<(std::ostream& s, const Array& v){
    bool first = true;
    s.put('[');
    L_Iter(v,it){
        if(first) first = false;
        else      s.put(',');
        s << (*it);
    }
    s.put(']');
    return s;
}
