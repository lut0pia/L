#include "Code.h"

#include "../burp.h"

#include "../regex.h"

using namespace L;
using namespace Burp;

Ref<Regex::Base> constRegex(Regex::from("\\-?[0-9]+(\\.[0-9]+)?|\".*\"|\\[.*\\]|true|false|null")),
                 intRegex(Regex::from("\\-?[0-9]{1,6}")),
                 ratioRegex(Regex::from("\\-?[0-9]+(\\.[0-9]+)?"));

bool Code::isConstExp(const String& code){
    return constRegex->matches(code);
}
Variable Code::fromConstExp(const String& str){
    if(str.size()>1 && str[0] == '"' && str[str.length()-1] == '"'){ // Is a String
        String tmp(str.begin()+1,str.end()-1);
        for(size_t i=0;i<tmp.size();i++){
            if(tmp[i]=='\\' && i+1<tmp.size())
                switch(tmp[i+1]){
                    case 'n': tmp.replace(i,2,"\n");break;
                    case 't': tmp.replace(i,2,"\t");break;
                    case '\\': tmp.replace(i,2,"\\");break;
                    default:break;
                }
        }
        return tmp;
    }
    else if(intRegex->matches(str)) // Is an int
        return atoi(str.c_str());
    else if(ratioRegex->matches(str)) // Is a Rational
        return Rational(str);
    else if(str == "false") // Is a bool
        return false;
    else if(str == "true")
        return true;
    else
        return Variable();
}

size_t Code::gIndex(const String& name, Map<String,size_t>& vi, size_t& ni){
    if(!vi.has(name))
        return vi[name] = ni++;
    return vi[name];
}
Ref<Code> Code::from(String code, Map<String,size_t>& vi, size_t& ni){
    size_t codeSize;

    while(code.size()){
        if(code[0]== '(' && code.endOf(0) == code.size()-1){ // (...) -> ...
            code.trimLeft();
            code.trimRight();
        }
        else if(code[0]==' ')code.trimLeft();
        else if(code[code.size()-1]==' ')code.trimRight();
        else break;
    }

    //std::cout << code << std::endl;

    codeSize = code.size();
    if(codeSize>1 && code[0]=='{' && code.endOf(0)==codeSize-1) // {...} : node
        return new Code_Node(code,vi,ni);
    else if(codeSize>1 && code[0]=='[' && code.endOf(0)==codeSize-1) // [...] : varNode
        return new Code_VarNode(code,vi,ni);
    else if(codeSize>3 && code.substr(0,3)=="if(") // if(a){...}[else{...}]
        return new Code_If(code,vi,ni);
    else if(codeSize>6 && code.substr(0,6)=="while(") // while(a){...}
        return new Code_While(code,vi,ni);
    else if(codeSize>4 && code.substr(0,4)=="for(") // for(a;b;c){...}
        return new Code_For(code,vi,ni);
    else{ // Expression
        List<String> part(code.escapedExplode(' ',String::allbrackets,3));
        size_t tmp;
        switch(part.size()){
            case 1: // Single eval or function call
                if((tmp = code.find_first_of('(')) != String::npos) // The expression is a basic function call
                    return new Code_FunctionCall(code,vi,ni);
                else if(isConstExp(code)) // The expression is a constant expression
                    return new Code_ConstValue(code,vi,ni);
                else if(code[0]=='&') // The expression is a reference to a variable
                    return new Code_VarRef(code,vi,ni);
                else // The expression is a variable
                    return new Code_Variable(code,vi,ni);
                break;
            case 2: // Unary operator
                return new Code_FunctionCall("."+part[0]+"("+part[1]+")",vi,ni);
                break;
            case 3:
                if(part[1]=="&&") // AND operator
                    return from("if("+part[0]+") "+part[2]+" else false",vi,ni);
                else if(part[1]=="||") // OR operator
                    return from("if("+part[0]+") true else "+part[2],vi,ni);
                else // Binary operator
                    return new Code_FunctionCall(".."+part[1]+"("+part[0]+","+part[2]+")",vi,ni);
                break;
            default: throw Exception("Burp: What.");
        }
    }

}
Array<Variable> Code::multiExecute(Array<Ref<Code> >& codes,Array<Variable>& locals){
    Array<Variable> wtr(codes.size());
    for(uint i(0);i<codes.size();i++)
        wtr[i] = codes[i]->execute(locals);
    return wtr;
}
