#ifndef DEF_L_stream
#define DEF_L_stream

#include <iostream>

namespace L{
    inline void nospace(std::istream& is){
        while(isspace(is.peek()))
            is.ignore();
    }
}

#endif

