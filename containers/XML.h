#ifndef DEF_L_XML
#define DEF_L_XML

#include "../containers/Map.h"
#include "../String.h"
#include "../containers/Array.h"
#include "../system/File.h"

namespace L {
  class XML {
    public:
      String name;
      Array<XML> children;
      Map<String,String> attributes;
      bool text;

      XML();
      XML(const File&);
      XML(Stream&);

      void write(Stream&) const;
      void read(Stream&);
  };
}

#endif



