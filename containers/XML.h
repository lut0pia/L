#ifndef DEF_L_XML
#define DEF_L_XML

#include "../containers/Map.h"
#include "../String.h"
#include "../containers/Array.h"

namespace L {
  class XML {
    public:
      String name;
      Array<XML> children;
      Map<String,String> attributes;
      bool text;

      XML();
      XML(const char*);
      XML(Stream&);

      void write(Stream&) const;
      void read(Stream&);
  };
}

#endif



