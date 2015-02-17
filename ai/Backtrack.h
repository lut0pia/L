#ifndef DEF_L_AI_Backtrack
#define DEF_L_AI_Backtrack

#include "../dynamic.h"

namespace L{
    namespace AI{
        class Backtrack{
            public:
                typedef struct{
                    bool (*test)(const Dynamic::Var&);
                    Dynamic::Var (*apply)(const Dynamic::Var&);
                } Rule;
            private:
                Set<Rule> rules;
            public:
                Backtrack(const Set<Rule>&);
                List<Rule> execute(const Dynamic::Var&);
        };
    }
}

#endif


