#include "Or.h"

using namespace L;
using namespace Regex;


Or::Or(size_t qMin, size_t qMax, const Array<Ref<Base> >& expressions) : Base(qMin,qMax), expressions(expressions) {}
Set<size_t> Or::accepts(const String& str, size_t i) {
  Set<size_t> tmp, wtr;
  tmp.insert(i);
  for(int i(0); i<expressions.size(); i++)
    wtr += expressions[i]->qAccepts(str,tmp);
  return wtr;
}
