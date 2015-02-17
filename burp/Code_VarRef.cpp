#include "Code_VarRef.h"

using namespace L;
using namespace Burp;

Code_VarRef::Code_VarRef(String code, Map<String,size_t>& vi, size_t& ni){
    id = gIndex(code.substr(1),vi,ni);
}
Variable Code_VarRef::execute(Vector<Variable>& locals){
    return &locals[id];
}

