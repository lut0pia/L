#pragma once

#include "Base.h"
#include "../containers/XML.h"

namespace L {
  namespace GUI {
    Ref<Base> from(const XML&);
    Ref<Base> from(const XML&, Map<String,Base*>&);

    Vector2i point(const String&);
  }
}
