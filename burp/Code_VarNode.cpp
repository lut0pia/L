#include "Code_VarNode.h"

using namespace L;
using namespace Burp;

Code_VarNode::Code_VarNode(String code, Map<String,size_t>& vi, size_t& ni) {
  code.trimLeft();    // [
  code.trimRight();   // ]
  List<String> part(code.escapedExplode(',',String::allbrackets));
  L_Iter(part,it)
  codes.push_back(from(*it,vi,ni));
}
Variable Code_VarNode::execute(Vector<Variable>& locals) {
  return multiExecute(codes,locals);
}

