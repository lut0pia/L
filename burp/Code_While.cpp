#include "Code_While.h"

using namespace L;
using namespace Burp;

Variable Code_While::execute(Array<Variable>& locals){
    Variable wtr;
    while(params[0]->execute(locals).get<bool>()){
        wtr = code->execute(locals);
    }
    return wtr;
}

