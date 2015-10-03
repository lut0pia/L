#include "Regex.h"

#include <algorithm>

#include "Char.h"
#include "Or.h"
#include "String.h"
#include "../stl/String.h"
#include "../Exception.h"
#include "../general.h"

using namespace L;
using namespace Regex;

typedef struct {size_t min,max;} quant_t;

quant_t quant(const L::String& str, size_t& i) {
  quant_t wtr = {1,1};
  size_t tmp;
  Array<L::String> part;
  if(i<str.size())
    switch(str[i]) {
      case '*':
        wtr.min = 0;
        wtr.max = -1;
        i++;
        break;
      case '+':
        wtr.max = -1;
        i++;
        break;
      case '?':
        wtr.min = 0;
        i++;
        break;
      case '{':
        tmp = 0;
        i++;
        while(str[(++tmp)+i]!='}') {}
        part = str.substr(i,tmp).explode(',');
        if(part.size()==2) {
          if(part[1]=="") { // {n,}
            wtr.min = FromString<size_t>(part[0]);
            wtr.max = -1;
          } else { // {n,m}
            wtr.min = FromString<size_t>(part[0]);
            wtr.max = FromString<size_t>(part[1]);
          }
        } else { // {n}
          wtr.min = wtr.max = FromString<size_t>(part[0]);
        }
        i += tmp+1;
        break;
      default:
        break;
    }
  return wtr;
}
Ref<Base> Regex::from(const L::String& str, size_t qMin, size_t qMax) {
  Array<Ref<Base> > exps;
  //out << str << " " << qMin << " " << qMax << '\n';
  if(!str.size()) throw Exception("Regex for empty string");  // Empty
  if(str==".") return new Char(qMin,qMax);                    // Any character
  if(str.size()==1) return new Char(qMin,qMax,str[0]);        // Single character
  if(str[0]=='[' && str.endOf(0)==str.size()-1) {             // Set of characters
    Set<char> chars;
    for(size_t i(1); i<str.size()-1; i++) {
      if(str[i]=='^') continue;
      else if(str[i]=='\\')
        chars.insert(str[++i]);
      else if(str[i]=='-')
        for(char c(std::min(str[i-1],str[i+1])); c<=std::max(str[i-1],str[i+1]); c++)
          chars.insert(c);
      else chars.insert(str[i]);
    }
    return new Char(qMin,qMax,chars);
  }
  if(str[0]=='(' && str.endOf(0)==str.size()-1)
    return from(str.substr(1,str.size()-2),qMin,qMax);
  Array<L::String> strs(str.escapedExplode('|',L::String::allbrackets));
  if(strs.size()>1) {
    exps.size(strs.size());
    for(uint i(0); i<strs.size(); i++)
      exps[i] = from(strs[i]);
    return new Or(qMin,qMax,exps);
  }
  size_t i(0);
  while(i<str.size()) {
    L::String sub;
    quant_t q;
    if(str[i]=='[' || str[i]=='(') {
      size_t tmp;
      sub = str.substr(i,(tmp = str.endOf(i)+1)-i);
      i = tmp;
      q = quant(str,i);
    } else {
      if(str[i]=='\\') i++;
      sub = L::String(1,str[i]);
      q = quant(str,++i);
    }
    exps.push(from(sub,q.min,q.max));
  }
  return (exps.size()>1) ? new String(qMin,qMax,exps) : exps[0];
}
