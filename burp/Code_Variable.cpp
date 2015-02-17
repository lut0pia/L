#include "Code_Variable.h"

using namespace L;
using namespace Burp;

Code_Variable::Code_Variable(String code, Map<String,size_t>& vi, size_t& ni){
    id = gIndex(code,vi,ni);
}
Variable Code_Variable::execute(Vector<Variable>& locals){
    return locals[id];
}

