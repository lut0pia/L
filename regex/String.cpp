#include "String.h"

using namespace L;
using namespace Regex;

Regex::String::String(size_t qMin, size_t qMax, const Vector<Ref<Base> >& expressions) : Base(qMin,qMax), expressions(expressions){}
Set<size_t> Regex::String::accepts(const L::String& str, size_t i){
    Set<size_t> wtr, tmp;

    tmp.insert(i);
    L_Iter(expressions,it){
        wtr = (*it)->qAccepts(str,tmp);
        tmp = wtr;
    }
    return tmp;
}
