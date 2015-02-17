#ifndef DEF_L_Logic_Formula
#define DEF_L_Logic_Formula

#include <cstdlib>
#include <vector>
#include "../containers.h"
#include "../Serializable.h"

namespace L{
    namespace Logic{
        class Formula : public Serializable{
            typedef enum{
                CONSTANT,VARIABLE,NOT,AND,OR
            } Type;
            private:
                Type type;
                union{
                    bool b;
                    size_t var;
                };
                Ref<Formula> p, q;
            public:
                Formula();
                Formula(bool);
                Formula(size_t);
                Formula operator!();
                Formula operator*(const Formula&);
                Formula operator+(const Formula&);
                bool operator==(const Formula&);
                bool interpret(const Vector<bool>&);
                void simplify();
                void write(std::ostream&) const;
                void read(std::istream&);

                static Formula random(size_t variables, size_t maxlevel);
        };
    }
}

#endif


