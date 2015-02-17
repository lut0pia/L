#include "Formula.h"

#include "../Rand.h"

using namespace L;
using namespace Logic;

Formula::Formula(): type(CONSTANT), b(false){}
Formula::Formula(bool b) : type(CONSTANT), b(b){}
Formula::Formula(size_t var) : type(VARIABLE), var(var){}
Formula Formula::operator!(){
    Formula wtr;
    wtr.type = NOT;
    wtr.p = new Formula(*this);
    return wtr;
}
Formula Formula::operator*(const Formula& f){
    Formula wtr;
    wtr.type = AND;
    wtr.p = new Formula(*this);
    wtr.q = new Formula(f);
    return wtr;
}
Formula Formula::operator+(const Formula& f){
    Formula wtr;
    wtr.type = OR;
    wtr.p = new Formula(*this);
    wtr.q = new Formula(f);
    return wtr;
}
bool Formula::operator==(const Formula& other){
    if(type == other.type)
        switch(type){
            case CONSTANT:  return (b == other.b);
            case VARIABLE:  return (var == other.var);
            case NOT:       return (*p == *other.p);
            case AND:
            case OR:        return ((*p == *other.p && *q == *other.q) || (*p == *other.q && *q == *other.p));
        }
    return false;
}
bool Formula::interpret(const Vector<bool>& variables){
    switch(type){
        case CONSTANT:  return b;
        case VARIABLE:  return variables[var];
        case NOT:       return !p->interpret(variables);
        case AND:       return p->interpret(variables) && q->interpret(variables);
        case OR:        return p->interpret(variables) || q->interpret(variables);
    }
    return false;
}
void Formula::simplify(){
    Formula tmp(*this);
    switch(type){
        case NOT:
            p->simplify();
            if(p->type == NOT)
                *this = *(p->p);
            else if(p->type == CONSTANT)
                *this = !p->b;
            break;
        case AND:
            p->simplify();
            q->simplify();
            if(p->type == CONSTANT)
                *this = ((p->b) ? *q : *p);
            else if(q->type == CONSTANT)
                *this = ((q->b) ? *p : *q);
            else if((p->type == NOT && *(p->p) == *q) || (q->type == NOT && *(q->p) == *p))
                *this = false;
            else if(*p == *q)
                *this = *p;
            break;
        case OR:
            p->simplify();
            q->simplify();
            if(p->type == CONSTANT)
                *this = ((p->b) ? *p : *q);
            else if(q->type == CONSTANT)
                *this = ((q->b) ? *q : *p);
            else if((p->type == NOT && *(p->p) == *q) || (q->type == NOT && *(q->p) == *p))
                *this = true;
            else if(*p == *q)
                *this = *p;
            break;
        default:break;
    }
}
void Formula::write(std::ostream& s) const{
    switch(type){
        case CONSTANT:  s << (b?'t':'f'); break;
        case VARIABLE:  s << var; break;
        case NOT:       s << '!'; p->write(s); break;
        case AND:       s << '('; p->write(s); s << '&'; q->write(s); s << ')'; break;
        case OR:        s << '('; p->write(s); s << '|'; q->write(s); s << ')'; break;
    }
}
void Formula::read(std::istream& s){

}

Formula Formula::random(size_t v, size_t ml){
    static size_t level;
    level++;
    Formula wtr;
    switch(wtr.type = (Type)Rand::next(CONSTANT,(level<ml)?OR:VARIABLE)){
        case CONSTANT:  wtr.b = Rand::next(0,1); break;
        case VARIABLE:  wtr.var = Rand::next(0,v-1); break;
        case NOT:       wtr.p = new Formula(random(v,ml)); break;
        case AND:
        case OR:        wtr.p = new Formula(random(v,ml)); wtr.q = new Formula(random(v,ml)); break;
    }
    level--;
    if(!level)
        wtr.simplify();
    return wtr;
}
