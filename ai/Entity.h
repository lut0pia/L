#ifndef DEF_L_AI_Entity
#define DEF_L_AI_Entity

#include <cstdlib>
#include <list>
#include "../logic.h"
#include "../stl.h"

namespace L{
    namespace AI{
        typedef Vector<Logic::Formula> Theory;
        class Entity : public Serializable{
            private:
                size_t inputSize, outputSize, memorySlots;
                Vector<bool> inMemory, outMemory;
                Vector<int> inAverage;
                Theory currentTheory;
                List<Theory> theories;
                Set<Set<size_t> > subsets;

            public:
                Entity(size_t inputSize,size_t outputSize,size_t memorySlots = 1);
                Vector<bool> action(const Vector<bool>&);
                void kill();

                void write(std::ostream&) const;
                void read(std::istream&);
        };
    }
}

#endif


