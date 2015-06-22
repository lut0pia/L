#ifndef DEF_L_Dynamic_Node
#define DEF_L_Dynamic_Node

#include "Variable.h"

namespace L {
  namespace Dynamic {
    class Node : public Map<String,Variable> {
      public:
        Node& operator()(const String&, const Variable&);
        friend Stream& operator<<(Stream&, const Node&);
    };
    Stream& operator<<(Stream& s, const Node& v);
  }
}

#endif
