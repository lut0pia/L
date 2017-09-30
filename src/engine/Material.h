#pragma once

#include "../container/KeyValue.h"
#include "../gl/Program.h"
#include "Resource.h"

namespace L {
  struct Material {
    Resource<GL::Program> program;
    Array<KeyValue<Symbol, Resource<GL::Texture>>> textures;
    void use();
  };
}
