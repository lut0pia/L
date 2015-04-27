#ifndef DEF_L_Dynamic_Node
#define DEF_L_Dynamic_Node

#include "Variable.h"

namespace L {
  namespace Dynamic {
    class Node : public Map<String,Variable> {
      public:
        Node& operator()(const String&, const Variable&);
        friend std::ostream& operator<<(std::ostream&, const Node&);
    };
    std::ostream& operator<<(std::ostream& s, const Node& v);
    L_TYPE_OUTABLE(Node)
  }
}

#endif
