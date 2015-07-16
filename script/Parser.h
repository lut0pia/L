#ifndef DEF_L_Script_Parser
#define DEF_L_Script_Parser

#include "Lexer.h"
#include "code.h"
#include "../containers/MultiArray.h"

namespace L {
  namespace Script {
    class Parser {
      private:
        class Node {
          public:
            uint _symbol;
            Array<Ref<Node> > _children;
            Node(uint);
            void addChild(Ref<Node>);
            void print();
        };
        Array<uint> _terminals, _nonterminals;
        Array<Array<uint> > _rules;
        MultiArray<2,uint> _table;
        uint _startSymbol;
      public:
        void startSymbol(const String&);
        void addRule(const Array<const char*>&);
        uint hash(const char*) const;
        uint index(uint hash);
        Ref<Parser::Node> parse(Lexer&);
    };
  }
}

#endif

