#ifndef DEF_L_ECS_Engine
#define DEF_L_ECS_Engine

#include "../stl/Set.h"

namespace L{
    namespace ECS{
        class Engine{
            private:
                static Set<void (*)()> updates;
            public:
                static void addUpdate(void (*)());
                static void update();
        };
    }
}

#endif

