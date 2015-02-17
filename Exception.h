#ifndef DEF_L_Exception
#define DEF_L_Exception

#include <exception>
#include "stl/String.h"

namespace L{
    class Exception : public std::exception{
        private:
            String w;
        public:
            Exception(const String& = "No explanation.");
            ~Exception() throw();
            virtual const char* what() const throw();
    };
}

#endif

