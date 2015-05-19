#include "String.h"

using namespace L;
using namespace Regex;

Regex::String::String(size_t qMin, size_t qMax, const Array<Ref<Base> >& expressions) : Base(qMin,qMax), expressions(expressions) {}
Set<size_t> Regex::String::accepts(const L::String& str, size_t i) {
  Set<size_t> wtr, tmp;
  tmp.insert(i);
  for(int i(0); i<expressions.size(); i++) {
    wtr = expressions[i]->qAccepts(str,tmp);
    tmp = wtr;
  }
  return tmp;
}
