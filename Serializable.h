#ifndef DEF_L_Serializable
#define DEF_L_Serializable

#include <iostream>
#include "tmp.h"

namespace L{
    class Serializable{
        public:
            void write(std::ostream&) const;
            void read(std::istream&);

            typedef int serializable;
            template <typename T> static void write(std::ostream& os, const T& s){s.write(os);}
            template <typename T> static void read(std::istream& is, T& s){s.read(is);}
            #define TMP(T) static void write(std::ostream& os, const T& s){os << s;} \
                           static void read(std::istream& is, T& s){is >> s;}
            TMP(char)
            TMP(int)
            TMP(uint)
            TMP(float)
            #undef TMP
    };

    template <typename T>
    class is_serializable{
        typedef char yes[1];
        typedef char no[2];

        template <typename R>
        static yes& test(typename R::serializable);
        template <typename>
        static no& test(...);

        public: static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
    };

    #define TMP(T) template <> class is_serializable<T>{public: static const bool value = true;}
    TMP(char);
    TMP(int);
    TMP(float);
    #undef TMP
}



#endif


