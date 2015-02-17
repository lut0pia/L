/*
#ifndef DEF_L_BurpVariable
#define DEF_L_BurpVariable

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include "../math.h"
#include "../Validate.h"
#include "../general.h"
#include "../stl.h"

namespace L{
    enum VarType {_notype,
                    _bool,
                    _int,
                    _number,
                    _string,
                    _array,
                    _pointer};

    class BurpVariable{
        private:
            union{
                void* p;
                int i;
                bool b;
            } value;
            unsigned char type;
            void free();

        public:

            BurpVariable();
            BurpVariable(const BurpVariable& b);
            ~BurpVariable();

            unsigned char gType() const;

            bool g_bool() const;
            int g_int() const;
            Rational& g_number() const;
            string& g_string() const;
            vector<BurpVariable>& g_array() const;
            BurpVariable* g_pointer() const;
            string typeName(bool extended = true) const;

            const BurpVariable& operator=(const BurpVariable& b);
            const BurpVariable& operator=(const bool& b);
            const BurpVariable& operator=(const int& b);
            const BurpVariable& operator=(const Rational& b);
            const BurpVariable& operator=(const string& b);
            const BurpVariable& operator=(const vector<BurpVariable>& b);
            const BurpVariable& points(BurpVariable* b);

            BurpVariable operator+(const BurpVariable& b) const;
            BurpVariable operator-(const BurpVariable& b) const;
            BurpVariable operator+() const;
            BurpVariable operator-() const;
            BurpVariable operator*(const BurpVariable& b) const;
            BurpVariable operator/(const BurpVariable& b) const;
            BurpVariable operator%(const BurpVariable& b) const;
            BurpVariable& operator ++();
            BurpVariable operator ++(int);
            BurpVariable& operator --();
            BurpVariable operator --(int);

            BurpVariable operator==(const BurpVariable& b) const;
            BurpVariable operator!=(const BurpVariable& b) const;
            BurpVariable operator>(const BurpVariable& b) const;
            BurpVariable operator<(const BurpVariable& b) const;
            BurpVariable operator>=(const BurpVariable& b) const;
            BurpVariable operator<=(const BurpVariable& b) const;

            BurpVariable indirection() const;
            BurpVariable reference() const;
    };
}
std::ostream& operator<<(std::ostream &stream, const L::BurpVariable& v);

#endif
*/


