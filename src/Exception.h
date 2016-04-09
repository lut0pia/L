#ifndef DEF_L_Exception
#define DEF_L_Exception

#include <exception>

namespace L {
  class Exception : public std::exception {
    private:
      const char* w;
    public:
      Exception(const char* = "No explanation.");
      ~Exception() throw(){}
      virtual const char* what() const throw() {return w;}
  };
}

#endif

