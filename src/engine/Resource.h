#pragma once

#include "../containers/Ref.h"
#include "../gl/Texture.h"
#include "../gl/Mesh.h"
#include "../script/Context.h"

namespace L {
  namespace Resource {
    const Ref<GL::Texture>& texture(const char* filepath);
    const Ref<GL::Mesh>& mesh(const char* filepath);
    const Ref<Script::CodeFunction>& script(const char* filepath);
  };
}
