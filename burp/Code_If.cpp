#include "Code_If.h"

#include "../Exception.h"

using namespace L;
using namespace Burp;

Code_If::Code_If(String code, Map<String,size_t>& vi, size_t& ni){
    size_t tmp, endof;
    if((endof = code.endOf(2))!=0){
        tmp = 0;
        codes[0] = from(code.substr(3,endof-3),vi,ni);
        // We have to test if there's an else or not
        if(code[endof+1] == '{'){
            tmp = 1+code.endOf(endof+1);
            if(code[tmp]==' ')tmp++;
            if(code.substr(tmp,4)!="else")tmp = 0;
        }
        else{
            tmp = code.find(String("else"));
            if(tmp==String::npos)tmp = 0;
        }
        hasElse = (tmp!=0);
        if(!hasElse) // No else
            codes[1] = from(code.substr(endof+1),vi,ni);
        else{ // There is an else
            codes[1] = from(code.substr(endof+1,tmp-endof-1),vi,ni);
            codes[2] = from(code.substr(tmp+4),vi,ni);
        }
    }
    else throw Exception("Burp: Syntax error in if");
}
Variable Code_If::execute(Array<Variable>& locals){
    if(codes[0]->execute(locals).get<bool>())
        return codes[1]->execute(locals);
    else if(hasElse)
        return codes[2]->execute(locals);
    return Variable();
}
