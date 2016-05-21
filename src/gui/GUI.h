#ifndef DEF_L_GUI
#define DEF_L_GUI

#include "Base.h"
#include "../containers/XML.h"

namespace L {
  namespace GUI {
    Ref<Base> from(const XML&);
    Ref<Base> from(const XML&, Map<String,Base*>&);

    Vector2i point(const String&);
  }
}


#endif



