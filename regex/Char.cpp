#include "Char.h"

using namespace L;
using namespace Regex;

Char::Char(size_t qMin, size_t qMax) : Base(qMin,qMax) {
  for(int c(-0x8F); c<=0x8F; c++)
    chars.insert(c);
}
Char::Char(size_t qMin, size_t qMax, char c) : Base(qMin,qMax) {
  chars.insert(c);
}
Char::Char(size_t qMin, size_t qMax, const Set<char>& chars) : Base(qMin,qMax), chars(chars) {}
Set<size_t> Char::accepts(const String& str, size_t i) {
  Set<size_t> wtr;
  if(i<str.size() && chars.has(str[i]))
    wtr.insert(i+1);
  return wtr;
}
