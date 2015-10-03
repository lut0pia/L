#ifndef DEF_L_Dynamic_Variable
#define DEF_L_Dynamic_Variable

#include "Cast.h"
#include "Type.h"

namespace L {
  namespace Dynamic {
    class Variable {
      private:
        union {
          int _data;
          void* _p;
        };
        const TypeDescription* _td;

        void* value();
        const void* value() const;

      public:
        Variable() : _data(0), _td(Type<int>::description()) {}
        template <class T> Variable(const T& v) : _td(Type<T>::description()) {
          if(local())  // Value is to be contained locally
            new(&_data) T(v);
          else _p = new T(v);
        }
        Variable(const char* s) : _td(Type<String>::description()) {
          if(local())
            new(&_data) String(s);
          else _p = new String(s);
        }
        Variable(const Variable& other);
        Variable& operator=(const Variable&);
        ~Variable();

        inline const TypeDescription* type() const {return _td;}
        inline bool local() const {return _td->size<=sizeof(_data);}

        template <class T> inline bool is() const {return _td == Type<T>::description();}
        template <class T> inline const T& as() const {return *(T*)value();}
        template <class T> inline T& as() {return *(T*)value();}

        inline bool canbe(TypeDescription* td2) const {return (Cast::get(_td, td2) != NULL);}
        template <class T> inline bool canbe() const {return canbe(Type<T>::description());}

        template <class T> T& get() {
          CastFct cast;
          if(is<T>())
            return as<T>();
          /*
          else if((cast = Cast::get(_td,Type<T>::description()))) {
          cast(value());
          return as<T>();
          }
          */
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

        template<class T> operator T() {return get<T>();}

        friend Stream& operator<<(Stream&, const Variable&);
    };
    Stream& operator<<(Stream&, const Variable&);
  }
  typedef Dynamic::Variable Var;
}

#endif
