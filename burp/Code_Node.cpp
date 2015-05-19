#include "Code_Node.h"

using namespace L;
using namespace Burp;

Code_Node::Code_Node(String code, Map<String,size_t>& vi, size_t& ni) {
  code.trimLeft();    // {
  code.trimRight();   // }
  if(code[code.size()-1]==' ')code.trimRight();
  if(code[code.size()-1]==';')code.trimRight();
  List<String> part(code.escapedExplode(';',String::allbrackets));
  L_Iter(part,it)
  codes.push(from(*it,vi,ni));
}
Variable Code_Node::execute(Array<Variable>& locals) {
  Variable wtr;
  for(size_t i=0; i<codes.size(); i++)
    wtr = codes[i]->execute(locals);
  return wtr;
}
