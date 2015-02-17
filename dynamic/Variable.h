#ifndef DEF_L_Dynamic_Variable
#define DEF_L_Dynamic_Variable

#include "Cast.h"
#include "Type.h"

namespace L{
    namespace Dynamic{
        class Variable{
            public:
                void* p;
                const TypeDescription* td;

            public:
                Variable() : p(NULL), td(Type<void>::description()){}
                template <class T> Variable(const T& v) : p(new T(v)), td(Type<T>::description()){}
                Variable(const char* s) : p(new String(s)), td(Type<String>::description()){}
                Variable(const Variable& other) : p(other.td->cpy(other.p)), td(other.td){}
                Variable& operator=(const Variable&);
                ~Variable(){td->del(p);}

                const TypeDescription* type() const{return td;}

                template <class T> bool is() const{return td == Type<T>::description();}

                template <class T> const T& as() const{return *(T*)p;}
                template <class T> T& as(){return *(T*)p;}

                bool canbe(TypeDescription* td2) const{
                    return (Cast::get(td,td2) != NULL);
                }
                template <class T> bool canbe() const{
                    return canbe(Type<T>::description());
                }

                template <class T> T& get(){
                    CastFct cast;
                    if(is<T>())
                        return as<T>();
                    else if((cast = Cast::get(td,Type<T>::description()))){
                        cast(p);
                        return as<T>();
                    }
                    else
                        return (*this = T()).as<T>();
                }

                bool operator==(const Variable&);
                bool operator!=(const Variable&);
                bool operator>(const Variable&);
                bool operator<(const Variable&);
                bool operator>=(const Variable&);
                bool operator<=(const Variable&);

                Variable& operator[](const String&);
                const Variable& operator[](const String&) const;
                Variable& operator[](size_t);

                template<class T> operator T(){return get<T>();}

                friend std::ostream& operator<<(std::ostream&, const Variable&);
        };
        typedef Variable Var;
        std::ostream& operator<<(std::ostream&, const Variable&);
    }
}

#endif
