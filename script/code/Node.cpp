#include "Node.h"

using namespace L;
using namespace Script;
using namespace Code;

Var Node::execute(Context& context) {
  for(int i(0); i<_codes.size(); i++)
    if(i<_codes.size()-1)
      _codes[i]->execute(context);
    else return _codes[i]->execute(context);
  return Var();
}
