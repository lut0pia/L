#ifndef DEF_L_Integer
#define DEF_L_Integer

#include <iostream>
#include <cmath>

#include "../stl.h"

namespace L{
    class Integer{
        private:
            typedef ulong type;
            static const size_t typebytes = sizeof(type);
            static const size_t typebits = typebytes*8;
            Vector<type> part; // Big-Endian
            bool negative;
        public:
            Integer();
            Integer(const long&);
            Integer(const String&, long base = 10);

            Integer operator+(const Integer&) const;
            Integer operator-(const Integer&) const;
            Integer operator+() const;
            Integer operator-() const;
            Integer operator*(const Integer&) const;
            Integer operator/(const Integer&) const;
            Integer operator%(const Integer&) const;

            Integer& operator++();
            Integer operator++(int);
            Integer& operator--();
            Integer operator--(int);

            bool operator==(const Integer&) const;
            bool operator!=(const Integer&) const;
            bool operator>(const Integer&) const;
            bool operator<(const Integer&) const;
            bool operator>=(const Integer&) const;
            bool operator<=(const Integer&) const;

            Integer operator<<(const size_t&) const;
            Integer operator>>(const size_t&) const;

            Integer& operator+=(const Integer&);
            Integer& operator-=(const Integer&);
            Integer& operator*=(const Integer&);
            Integer& operator/=(const Integer&);
            Integer& operator%=(const Integer&);
            Integer& operator<<=(const size_t&);
            Integer& operator>>=(const size_t&);

            Integer abs() const;
            String toShortString() const;
            String toString(long base = 10) const;

        private:
            type gPart(size_t) const;
            void sPart(size_t,type);
            void reset();
            void trim();
            size_t size() const{return part.size();}

        public :
            static Integer lcd(const Integer& a, const Integer& b);
            static Integer gcd(Integer a, Integer b);
            static Integer pow(const Integer& a, Integer b);
        friend class Rational;
        friend std::ostream& operator<<(std::ostream&, const Integer&);
    };

    std::ostream& operator<<(std::ostream&, const Integer&);
}

#endif





