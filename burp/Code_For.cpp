#include "Code_For.h"

using namespace L;
using namespace Burp;

Variable Code_For::execute(Vector<Variable>& locals){
    Variable wtr;
    for(params[0]->execute(locals);
        params[1]->execute(locals).get<bool>();
        params[2]->execute(locals)){
        wtr = code->execute(locals);
    }
    return wtr;
}


