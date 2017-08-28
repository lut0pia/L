#include "Material.h"

using namespace L;

void Material::use() {
  if(program) {
    program->use();
    for(uintptr_t i(0); i<textures.size(); i++)
      if(textures[i].value())
        program->uniform(textures[i].key(), *textures[i].value(), GL_TEXTURE0+i);
  }
}
