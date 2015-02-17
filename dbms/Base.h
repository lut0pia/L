#ifndef DEF_L_DBMS_Base
#define DEF_L_DBMS_Base

#include "../stl.h"
#include "../system.h"
#include "Table.h"

namespace L{
    namespace DBMS{
        class Base{
            private:
                Map<String,Table> table;

            public:
                //Base(const Directory&);
                //void query(const String&);
        };
    }
}

#endif
