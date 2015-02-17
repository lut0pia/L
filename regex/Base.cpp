#include "Base.h"

using namespace L;
using namespace Regex;

Base::Base(size_t qMin, size_t qMax) : qMin(qMin), qMax(qMax){}
Set<size_t> Base::qAccepts(const String& str, const Set<size_t>& start, size_t count){
    Set<size_t> wtr;
    if(qMin<=count) wtr += start;

    if(qMax>count)
        L_Iter(start,it){
            Set<size_t> newStart(accepts(str,*it));
            if(!newStart.empty())
                wtr += qAccepts(str,newStart,count+1);
        }

    return wtr;
}
bool Base::matches(const String& str){
    Set<size_t> start;
    start.insert(0);
    return qAccepts(str,start).has(str.size());
}
