#ifndef DEF_L_Burp_Code_Super
#define DEF_L_Burp_Code_Super

#include "Code.h"

namespace L {
  namespace Burp {
    template <size_t idSize,size_t n>
    class Code_Super : public Code {
      protected:
        Ref<Code> params[n];
        Ref<Code> code;

      public:
        Code_Super(String code, Map<String,size_t>& vi, size_t& ni) {
          size_t tmp;
          if((tmp = code.endOf(idSize))!=0) { // id(...)...
            List<String> part(code.substr(idSize+1,tmp-(idSize+1)).explode(';'));
            size_t i(0);
            L_Iter(part,it) this->params[i++] = from(*it,vi,ni); // Retrieve super param
            this->code = from(code.substr(tmp+1),vi,ni); // Retrieve super code
          } else throw Exception("Burp: Syntax error in super");
        }
    };
  }
}

#endif






