/*
#include "BurpVariable.h"

using namespace L;

void BurpVariable::free(){
    switch(type){
        case _number:   delete (Number*)value.p; break;
        case _string:   delete (string*)value.p; break;
        case _array:   delete (vector<BurpVariable>*)value.p; break;
        default:        break;//ya neck
    }
    type = _notype;
    value.p = NULL;
}

BurpVariable::BurpVariable(){
    type = _notype;
    value.p = NULL;
}
BurpVariable::BurpVariable(const BurpVariable& b){
    type = _notype;
    value.p = NULL;
    *this = b;
}
BurpVariable::~BurpVariable(){
    free();
}

unsigned char BurpVariable::gType() const{
    return type;
}

bool BurpVariable::g_bool() const{
    return (gType()==_bool && value.b);
}
int BurpVariable::g_int() const{
    return value.i;
}
Number& BurpVariable::g_number() const{
    return *(Number*)value.p;
}
string& BurpVariable::g_string() const{
    return *(string*)value.p;
}
vector<BurpVariable>& BurpVariable::g_array() const{
    return *(vector<BurpVariable>*)value.p;
}
BurpVariable* BurpVariable::g_pointer() const{
    return (BurpVariable*)value.p;
}
string BurpVariable::typeName(bool extended) const{
    switch(type){
        case _bool:     return "bool"; break;
        case _int:      return "int"; break;
        case _number:   return "number"; break;
        case _string:   return "string"; break;
        case _array:   return "vector"; break;
        case _pointer:  return "*:"+g_pointer()->typeName(false); break;
        default:        return "notype"; break;
    }
}

const BurpVariable& BurpVariable::operator=(const BurpVariable& b){
    if(&b != this){
        switch(b.type){
            case _bool:     *this = b.g_bool(); break;
            case _int:      *this = b.g_int(); break;
            case _number:   *this = b.g_number(); break;
            case _string:   *this = b.g_string(); break;
            case _array:   *this = b.g_array(); break;
            case _pointer:  points(b.g_pointer()); break;
            default:        free(); break;
        }
    }
    return b;
}
const BurpVariable& BurpVariable::operator=(const bool& b){
    free();
    type = _bool;
    value.b = b;
    return *this;
}
const BurpVariable& BurpVariable::operator=(const int& b){
    free();
    type = _int;
    value.i = b;
    return *this;
}
const BurpVariable& BurpVariable::operator=(const Number& b){
    free();
    type = _number;
    value.p = new Number(b);
    return *this;
}
const BurpVariable& BurpVariable::operator=(const string& b){
    free();
    type = _string;
    value.p = new string(b);
    return *this;
}
const BurpVariable& BurpVariable::operator=(const vector<BurpVariable>& b){
    free();
    type = _array;
    value.p = new vector<BurpVariable>(b);
    return *this;
}
const BurpVariable& BurpVariable::points(BurpVariable* b){
    free();
    type = _pointer;
    value.p = b;
    return *this;
}

BurpVariable BurpVariable::operator+(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _bool:     wtr = g_bool() || b.g_bool(); break;
            case _int:      wtr = g_int() + b.g_int(); break;
            case _number:   wtr = g_number() + b.g_number(); break;
            case _string:   wtr = g_string() + b.g_string(); break;
            case _array:    wtr = g_array() + b.g_array(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() + Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) + b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator-(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:      wtr = g_int() - b.g_int(); break;
            case _number:   wtr = g_number() - b.g_number(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() - Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) - b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator+() const{
    return *this;
}
BurpVariable BurpVariable::operator-() const{
    BurpVariable wtr;
    switch(type){
        case _int:      wtr = -g_int();
        case _number:   wtr = -g_number();
        default:break;
    }
    return wtr;
}
BurpVariable BurpVariable::operator*(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _bool:     wtr = g_bool() && b.g_bool(); break;
            case _int:      wtr = g_int() * b.g_int(); break;
            case _number:   wtr = g_number() * b.g_number(); break;
            case _string:   wtr = g_string() + b.g_string(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() * Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) * b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator/(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:
                if(b.g_int() != 0)
                    wtr = g_int() / b.g_int();
                break;
            case _number:
                if(b.g_number() != Number())
                    wtr = g_number() / b.g_number();
                break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int && b.g_int() != 0)
        wtr = g_number() / Number(b.g_int());
    else if(type==_int && b.type==_number && b.g_number() != Number())
        wtr = Number(g_int()) / b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator%(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:
                if(b.g_int() != 0)
                    wtr = g_int() % b.g_int();
                break;
            case _number:
                if(b.g_number() != Number())
                    wtr = g_number() % b.g_number();
                break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int && b.g_int() != 0)
        wtr = g_number() % Number(b.g_int());
    else if(type==_int && b.type==_number && b.g_number() != Number())
        wtr = Number(g_int()) % b.g_number();
    return wtr;
}

BurpVariable& BurpVariable::operator ++(){
    if(type==_number)
        *this = ++g_number();
    else if(type==_int)
        *this = g_int()+1;
    return *this;
}
BurpVariable BurpVariable::operator ++(int){
    BurpVariable wtr = *this;
    if(type==_number)
        *this = ++g_number();
    else if(type==_int)
        *this = g_int()+1;
    return wtr;
}
BurpVariable& BurpVariable::operator --(){
    if(type==_number)
        *this = --g_number();
    else if(type==_int)
        *this = g_int()-1;
    return *this;
}
BurpVariable BurpVariable::operator --(int){
    BurpVariable wtr = *this;
    if(type==_number)
        *this = --g_number();
    else if(type==_int)
        *this = g_int()-1;
    return wtr;
}

BurpVariable BurpVariable::operator==(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _bool:     wtr = g_bool() == b.g_bool(); break;
            case _int:      wtr = g_int() == b.g_int(); break;
            case _number:   wtr = g_number() == b.g_number(); break;
            case _string:   wtr = g_string() == b.g_string(); break;
            case _array:
                if(g_array().size()!=b.g_array().size())
                    wtr = false;
                else{
                    for(size_t i=0;i<g_array().size();i++)
                        if((g_array()[i]!=b.g_array()[i]).g_bool())
                            wtr = false;
                    wtr = true;
                }
                break;
            case _pointer:  wtr = g_pointer() == b.g_pointer(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() == Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) == b.g_number();
    else wtr = false;
    return wtr;
}
BurpVariable BurpVariable::operator!=(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _bool:     wtr = g_bool() != b.g_bool(); break;
            case _int:      wtr = g_int() != b.g_int(); break;
            case _number:   wtr = g_number() != b.g_number(); break;
            case _string:   wtr = g_string() != b.g_string(); break;
            case _array:
                if(g_array().size()!=b.g_array().size())
                    wtr = true;
                else{
                    for(size_t i=0;i<g_array().size();i++)
                        if((g_array()[i]!=b.g_array()[i]).g_bool())
                            wtr = true;
                    wtr = false;
                }
                break;
            case _pointer:  wtr = g_pointer() != b.g_pointer(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() != Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) != b.g_number();
    else wtr = true;
    return wtr;
}
BurpVariable BurpVariable::operator>(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:      wtr = g_int() > b.g_int(); break;
            case _number:   wtr = g_number() > b.g_number(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() > Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) > b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator<(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:      wtr = g_int() < b.g_int(); break;
            case _number:   wtr = g_number() < b.g_number(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() < Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) < b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator>=(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:      wtr = g_int() >= b.g_int(); break;
            case _number:   wtr = g_number() >= b.g_number(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() >= Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) >= b.g_number();
    return wtr;
}
BurpVariable BurpVariable::operator<=(const BurpVariable& b) const{
    BurpVariable wtr;
    if(type == b.type){
        switch(type){
            case _int:      wtr = g_int() <= b.g_int(); break;
            case _number:   wtr = g_number() <= b.g_number(); break;
            default:break;
        }
    }
    else if(type==_number && b.type==_int)
        wtr = g_number() <= Number(b.g_int());
    else if(type==_int && b.type==_number)
        wtr = Number(g_int()) <= b.g_number();
    return wtr;
}

BurpVariable BurpVariable::indirection() const{
    BurpVariable* p;
    return ((p = g_pointer()) != NULL) ? *p : BurpVariable();
}
BurpVariable BurpVariable::reference() const{
    BurpVariable wtr;
    wtr.points((BurpVariable*)this);
    return wtr;
}

ostream& operator<<(ostream &stream, const BurpVariable& v){
    vector<BurpVariable> tmp;
    switch(v.gType()){
        case _bool:     stream << (v.g_bool() ? "true" : "false"); break;
        case _int:      stream << v.g_int(); break;
        case _number:   stream << v.g_number(); break;
        case _string:   stream << v.g_string(); break;
        case _array:
            tmp = v.g_array();
            stream << "{";
            for(size_t i=0;i<tmp.size();i++)
                stream << tmp[i] << ((i<tmp.size()-1) ? "," : "");
            stream << "}";
            break;
        case _pointer:  stream << "*" << *v.g_pointer(); break;
        default: stream << "N/A"; break;
    }
    return stream;
}
*/
