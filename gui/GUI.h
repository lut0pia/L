#ifndef DEF_L_GUI
#define DEF_L_GUI

#include "Base.h"
#include "../containers/XML.h"

namespace L {
  namespace GUI {
    Base* from(const XML&);
    Base* from(const XML&, Map<String,Base*>&);

    Point2i point(const String&);
  }
}


#endif



