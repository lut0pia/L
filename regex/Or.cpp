#include "Or.h"

using namespace L;
using namespace Regex;


Or::Or(size_t qMin, size_t qMax, const Vector<Ref<Base> >& expressions) : Base(qMin,qMax), expressions(expressions){}
Set<size_t> Or::accepts(const String& str, size_t i){
    Set<size_t> tmp, wtr;
    tmp.insert(i);
    L_Iter(expressions,it)
        wtr += (*it)->qAccepts(str,tmp);
    return wtr;
}
